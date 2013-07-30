packages = %w{wget python-devel git subversion}

home = "/home/vagrant"
nocredentials = <<-EOH
svn co --ignore-externals http://svn.code.sf.net/p/paintown/code/www paintown
chmod -R 755 #{home}
EOH
withcredentials = <<-EOH
svn co --username #{node.include?('paintown') ? node["paintown"]["username"] : ""} --password #{node.include?('paintown') ? node["paintown"]["password"]: ""} --ignore-externals svn+ssh://#{node.include?('paintown') ? node["paintown"]["username"] : ""}@svn.code.sf.net/p/paintown/code/www paintown
chmod -R 755 #{home}
EOH

packages.each do |pkg|
  package pkg do
    action :install
  end
end

bash "save-iptables" do
  user "root"
  code <<-EOH
    iptables-save > /root/default.iptables
  EOH
end

ruby_block "edit-files" do
    block do
        rc = Chef::Util::FileEdit.new('/root/default.iptables')
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A INPUT -p tcp -m tcp --dport 80 -j ACCEPT'
        )
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A INPUT -p tcp -m tcp --dport 8080 -j ACCEPT'
        )
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A INPUT -p tcp -m tcp --dport 443 -j ACCEPT'
        )
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A OUTPUT -p tcp -m tcp --sport 80 -j ACCEPT'
        )
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A OUTPUT -p tcp -m tcp --sport 8080 -j ACCEPT'
        )
        rc.insert_line_after_match(
            /^:OUTPUT ACCEPT.*$/,
            '-A OUTPUT -p tcp -m tcp --sport 443 -j ACCEPT'
        )
        rc.write_file
        
        rc = Chef::Util::FileEdit.new('/etc/httpd/sites-available/default')
        rc.search_file_replace(
            /\/var\/www/,
            '/home/vagrant/paintown'
        )
        rc.write_file
    end
end

bash "reload-iptables" do
  user "root"
  code <<-EOH
    iptables-restore /root/default.iptables
    /etc/init.d/iptables save
    service iptables restart
    #setenforce 0
  EOH
end

bash "co-paintown" do
  user "vagrant"
  cwd home
  code node.include?('paintown') ? withcredentials : nocredentials
end
