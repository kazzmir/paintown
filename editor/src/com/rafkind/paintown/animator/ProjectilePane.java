package com.rafkind.paintown.animator;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;

import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.Lambda2;

import com.rafkind.paintown.RelativeFileChooser;

import org.swixml.SwingEngine;

public class ProjectilePane {

	private JPanel mainPanel;
	private Projectile projectile;
	
	private Animation currentAnimation;

    public ProjectilePane(final NewAnimator animator, final Projectile projectile){
        this.projectile = projectile;
        
        SwingEngine engine = new SwingEngine("animator/projectile.xml");

        final JTabbedPane tabs = (JTabbedPane) engine.find("tabs");

        for (Animation animation: projectile.getAnimations()){
            tabs.add(new ProjectileCanvas(projectile, animation), animation.getName());
        }
        
        mainPanel = (JPanel) engine.getRootComponent();
    }

    private class ProjectileCanvas extends AnimationCanvas {
        ProjectileCanvas(Projectile projectile, Animation animation){
            super(projectile, animation, new Lambda2(){
                public Object invoke(Object o1, Object o2){
                    return null;
                }
            });
        }
    
        protected JComponent makeProperties(final AnimatedObject object, final Animation animation, final Lambda2 changeName){

            final SwingEngine contextEditor = new SwingEngine("animator/projectile-properties.xml");

            final JTextField basedirField = (JTextField) contextEditor.find("basedir");
            {
                Dimension size = basedirField.getMinimumSize();
                size.setSize(9999999, size.getHeight());
                basedirField.setMaximumSize(size);
            }
            basedirField.setText(animation.getBaseDirectory());
            JButton basedirButton = (JButton) contextEditor.find("change-basedir");
            basedirButton.addActionListener(new AbstractAction(){
                public void actionPerformed(ActionEvent event){
                    RelativeFileChooser chooser = NewAnimator.getNewFileChooser(object.getPath());
                    int ret = chooser.open();
                    if (ret == RelativeFileChooser.OK){
                        final String path = chooser.getPath();
                        basedirField.setText(path);
                        animation.setBaseDirectory(path);
                    }
                }
            });

            return (JComponent) contextEditor.getRootComponent();
        }
    }

	public SpecialPanel getEditor(){
		return new SpecialPanel(mainPanel, null, projectile);
	}
}
