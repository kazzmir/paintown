package com.rafkind.paintown.animator;

import javax.swing.*;

import org.swixml.SwingEngine;

public class ProjectilePane {

	private JPanel mainPanel;
	private JTextField nameField;
	private Projectile projectile;

	public ProjectilePane( final Animator animator, final Projectile projectile ){
		this.projectile = projectile;

		SwingEngine engine = new SwingEngine( "animator/projectile.xml" );
		
		final DrawArea drawArea = new DrawArea();
		JPanel canvas = (JPanel) engine.find( "canvas" );
		canvas.add( drawArea );

		nameField = (JTextField) engine.find( "name" );

		nameField.setText( projectile.getName() );

		final JButton start = (JButton) engine.find( "start" );
		final JButton stop = (JButton) engine.find( "stop" );
		final JButton edit = (JButton) engine.find( "edit" );

		final JList animations = (JList) engine.find( "animations" );
		
		mainPanel = (JPanel) engine.getRootComponent();
	}

	public SpecialPanel getEditor(){
		return new SpecialPanel( mainPanel, nameField, projectile );
	}

}
