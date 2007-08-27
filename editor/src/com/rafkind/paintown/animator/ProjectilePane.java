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
		
		mainPanel = (JPanel) engine.getRootComponent();
	}

	public SpecialPanel getEditor(){
		return new SpecialPanel( mainPanel, nameField, projectile );
	}

}
