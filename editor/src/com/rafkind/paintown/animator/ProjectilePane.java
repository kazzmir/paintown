package com.rafkind.paintown.animator;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;

import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.Lambda2;

import org.swixml.SwingEngine;

public class ProjectilePane {

	private JPanel mainPanel;
	private Projectile projectile;
	
	private Animation currentAnimation;

    public ProjectilePane(final NewAnimator animator, final Projectile projectile){
        this.projectile = projectile;

        SwingEngine engine = new SwingEngine("animator/projectile.xml");

        final DrawArea drawArea = new DrawArea(projectile.getDrawProperties(), new Lambda0(){
            public Object invoke(){
                return null;
            }
        });

        JPanel canvas = (JPanel) engine.find("canvas");
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;
        canvas.add(drawArea, constraints);

        final JButton start = (JButton) engine.find("start");
        final JButton stop = (JButton) engine.find("stop");
        final JButton edit = (JButton) engine.find("edit");

        final JList animations = (JList) engine.find("animations");

        animations.setListData(projectile.getAnimations());

        final Lambda1 editAnimation = new Lambda1(){
            public Object invoke(Object i){
                int index = ((Integer) i).intValue();
                Animation temp = projectile.getAnimation(index);
                CharacterAnimation edit = new CharacterAnimation( projectile, temp, new Lambda2(){
                    public Object invoke(Object o1, Object o2){
                        return null;
                    }
                });
                // animator.addNewTab( edit.getEditor(), temp.getName() );
                return null;
            }
        };

        animations.addMouseListener( new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                if (e.getClickCount() == 2) {
                    int index = animations.locationToIndex(e.getPoint());
                    editAnimation.invoke_(new Integer(index));
                }
            }
        });

        edit.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                editAnimation.invoke_(new Integer(animations.getSelectedIndex()));
            }
        });

        start.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if (animations.getSelectedValue() != null){
                    if (currentAnimation != null){
                        currentAnimation.stopRunning();
                    }
                    currentAnimation = (Animation) animations.getSelectedValue();
                    System.out.println("Start animation " + currentAnimation);
                    drawArea.animate(currentAnimation);
                    currentAnimation.startRunning();
                }
            }
        });

        stop.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                drawArea.unanimate();
                if (currentAnimation != null){
                    currentAnimation.stopRunning();
                }
            }
        });

        final JLabel scaleNum = (JLabel) engine.find("scale-num");
        scaleNum.setText( "Scale: " + drawArea.getScale() );
        final JSlider scale = (JSlider) engine.find( "scale" );
        scale.setValue( (int)(drawArea.getScale() * 5.0) );
        scale.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                drawArea.setScale( scale.getValue() / 5.0 );
                scaleNum.setText( "Scale: " + drawArea.getScale() );
            }
        });

        mainPanel = (JPanel) engine.getRootComponent();
    }

	public SpecialPanel getEditor(){
		return new SpecialPanel(mainPanel, null, projectile);
	}
}
