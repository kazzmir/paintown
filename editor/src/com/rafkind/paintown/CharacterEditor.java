package com.rafkind.paintown;

import javax.swing.*;
import javax.swing.event.*;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

import com.rafkind.paintown.level.Character;
import com.rafkind.paintown.level.Level;
import com.rafkind.paintown.level.Block;

import org.swixml.SwingEngine;

public class CharacterEditor implements PropertyEditor {
	private Character character;
	public CharacterEditor( Character i ){
		this.character = i;
	}

	private int findBlock( Level level ){
		int i = 1;
		for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.hasThing( character ) ){
				return i;
			}
			i += 1;
		}
		return i;
	}

	public JComponent createPane( final Level level ){
		final SwingEngine engine = new SwingEngine( "character.xml" );
		final JTextField name = (JTextField) engine.find( "name" );
		name.setText( character.getName() );
		final JTextField health = (JTextField) engine.find( "health" );
		health.setText( String.valueOf( character.getHealth() ) );
		final JTextField x = (JTextField) engine.find( "x" );
		x.setText( String.valueOf( character.getX() ) );
		final JTextField y = (JTextField) engine.find( "y" );
		y.setText( String.valueOf( character.getY() ) );
		final JSpinner block = (JSpinner) engine.find( "block" );
		block.setModel( new SpinnerModel(){
			int value = findBlock( level );
			List listeners = new ArrayList();
			public Object getNextValue(){
				int max = level.getBlocks().size();
				return max > value ? new Integer( value + 1 ) : null;
			}

			public void addChangeListener( ChangeListener l ){
				listeners.add( l );
			}

			public void removeChangeListener( ChangeListener l ){
				listeners.remove( l );
			}

			public Object getPreviousValue(){
				return value > 1 ? new Integer( value - 1 ) : null;
			}

			public void setValue( Object o ){
				value = ((Integer) o).intValue();
				ChangeEvent event = new ChangeEvent( this );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ChangeListener change = (ChangeListener) it.next();
					change.stateChanged( event );
				}
			}

			public Object getValue(){
				return new Integer( value );
			}
		});
		return (JPanel) engine.getRootComponent();
	}
}
