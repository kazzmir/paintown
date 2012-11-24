package com.rafkind.paintown;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;

import java.util.List;
import java.util.Collections;
import java.util.Iterator;
import java.util.ArrayList;
import java.io.File;

import org.swixml.SwingEngine;

public class RelativeFileChooser extends JDialog {

	public static final int OK = 0;
	public static final int CANCEL = 1;

	private List<File> paths = new ArrayList<File>();
	private int option = CANCEL;
	
    public RelativeFileChooser(JFrame frame, String title, File start){
        this(frame, title, start, null);
    }

	public RelativeFileChooser(JFrame frame, String title, final File start, File jump){
		super(frame, title, true);
		this.setSize(300, 400);

		SwingEngine engine = new SwingEngine("relative.xml");
		JPanel panel = (JPanel) engine.getRootComponent();
		this.getContentPane().add(panel);

		this.addWindowListener(new WindowAdapter(){
			public void windowClosing(WindowEvent e){
				setOption(CANCEL);
			}
		});
		
		class FileList implements ListModel{
			List listeners = new ArrayList();
			List files = new ArrayList();

			public FileList(File start){
				setFile(start);
			}

			public void setFile(File file){
				files = new ArrayList();
				if (file.isDirectory()){
					File[] all = file.listFiles();
                    files.add(new File("."));
					files.add(new File(".."));
					for (int i = 0; i < all.length; i++){
						files.add(all[i]);
					}
				}

				ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 999999 );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.contentsChanged(event);
				}
			}

			public void removeListDataListener(ListDataListener l){
				listeners.remove(l);
			}

			public void addListDataListener(ListDataListener l){
				listeners.add(l);
			}

			public Object getElementAt(int index){
				return files.get(index);
			}

			public int getSize(){
				return files.size();
			}
		};

        File use = start;
        if (jump != null){
            use = jump;
            if (use.isFile()){
                use = use.getParentFile();
            }
        }

        // paths.add( start );
		paths.add(new File("."));
        paths.addAll(pathTo(start, use));
		
        final JTextField path = (JTextField) engine.find("path");
		path.setText(getPath());

		final FileList list = new FileList(use);

		final JList files = (JList) engine.find("files");
		files.setModel(list);

		files.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "select");

        final Lambda1 updateFile = new Lambda1(){
            public Object invoke(Object o){
                File file = (File) o;

                /* Don't do anything if the user selects the current directory */
                if (file.toString() == "."){
                    return null;
                }

                /* If they select '..' then remove one path */
                if (file.toString() == ".." && paths.size() > 1){
                    paths.remove(paths.size() - 1);
                    path.setText(getPath());
                } else {
                    /* Otherwise add the path to the current list */
                    paths.add(file);
                }

                list.setFile(getTruePath(start));
                path.setText(getPath());

                /* User has selected a file so theres nothing to do except return it to the user */
                if (! file.isDirectory()){
                    setOption(OK);
                    RelativeFileChooser.this.setVisible(false);
                }

                /* Reset the visible list */
                if (list.getSize() > 0){
                    files.setSelectedIndex(0);
                    files.ensureIndexIsVisible(0);
                }

                return null;
            }
        };

		files.getActionMap().put("select", new AbstractAction(){
			public void actionPerformed(ActionEvent event){
				File file = (File) files.getSelectedValue();
                updateFile.invoke_(file);
			}
		});

		files.addMouseListener(new MouseAdapter(){
			public void mouseClicked(MouseEvent clicked){
				if (clicked.getClickCount() == 2){
					Action action = files.getActionMap().get("select");
					action.actionPerformed(new ActionEvent(this, 0, "select"));
				}
			}
		});

		final JButton up = (JButton) engine.find("up");
		up.addActionListener( new AbstractAction(){
			public void actionPerformed(ActionEvent e){
                updateFile.invoke_(new File(".."));
                /*
				if (paths.size() > 1){
					paths.remove(paths.size() - 1);
					list.setFile(getTruePath(start));
					path.setText(getPath());
				}
                */
			}
		});

		final JButton cancel = (JButton) engine.find("cancel");
		cancel.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent e){
				setOption(CANCEL);
				RelativeFileChooser.this.setVisible(false);
			}
		});

		final JButton ok = (JButton) engine.find("ok");
		ok.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent e){
                File selected = (File) files.getSelectedValue();
                if (selected.getName() == ".."){
                    /* Don't allow user to select .. */
                    return;
                } else if (selected.getName() == "."){
                    /* If the user selects . then they want this directory */
                } else {
                    /* Otherwise they selected a file/directory so use that one */
                    paths.add(selected);
                }

                /* User has made their choice so we are done */
				setOption(OK);
				RelativeFileChooser.this.setVisible(false);
			}
		});
	}

    /* discovers the relative path between 'base' and 'find'.
     * base: /a/b/c
     * find: /a/b/c/d/e/f
     * output: d/e/f
     */
    private List<File> pathTo(File base, File find){
        if (find == null){
            return new ArrayList<File>();
        }

        List<File> out = new ArrayList<File>();
        while (!find.equals(base)){
            /* add the current find path then go one up and loop */

            if (find.isDirectory()){
                out.add(find);
            }
            File parent = find.getParentFile();
            /* if the parent is the same as the current directory then we have
             * hit the root and have implicitly gone past the base since the
             * base should never be the root anyway.
             */
            if (parent == null || parent.equals(find)){
                return new ArrayList<File>();
            }
            find = parent;
        }
        /* we started adding from the front of 'find' but that gives the
         * wrong order, so reverse it
         */
        Collections.reverse(out);

        return out;
    }

	private void setOption(int i){
		this.option = i;
	}

	private int getOption(){
		return this.option;
	}

	public int open(){
		this.show();
		return getOption();
	}

    public File getTruePath(File start){
        for (File file: paths){
            start = new File(start, file.getName());
        }
        return start;
    }

	public String getPath(){
		StringBuffer b = new StringBuffer();
		for (Iterator it = paths.iterator(); it.hasNext(); ){
			File file = (File) it.next();
			b.append(file.getName());
			if (file.isDirectory()){
				b.append("/");
			}
		}
		return b.toString();
	}

}
