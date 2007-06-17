package com.rafkind.paintown;

import javax.swing.*;

import com.rafkind.paintown.level.Character;
import com.rafkind.paintown.level.Level;

import org.swixml.SwingEngine;

public class CharacterEditor implements PropertyEditor {
	public CharacterEditor( Character i ){
	}

	public JComponent createPane( Level level ){
		final SwingEngine engine = new SwingEngine( "character.xml" );
		return (JPanel) engine.getRootComponent();
	}
}
