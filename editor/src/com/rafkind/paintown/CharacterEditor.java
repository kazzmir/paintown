package com.rafkind.paintown;

import java.awt.event.*;
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

	private Block getBlock( int num, Level level ){
		return (Block) level.getBlocks().get( num - 1 );
	}

	public JComponent createPane( final Level level, final Lambda0 closeProc ){
		final SwingEngine engine = new SwingEngine( "character.xml" );
		final JTextField name = (JTextField) engine.find( "name" );
		name.setText( character.getName() );
		final JTextField health = (JTextField) engine.find( "health" );
		health.setText( String.valueOf( character.getHealth() ) );
		final JTextField x = (JTextField) engine.find( "x" );
		x.setText( String.valueOf( character.getX() ) );
		final JTextField y = (JTextField) engine.find( "y" );
		y.setText( String.valueOf( character.getY() ) );
		final JTextField path = (JTextField) engine.find( "path" );
		path.setText( character.getPath() );
		path.setEditable( false );
		final JTextField aggression = (JTextField) engine.find( "aggression" );
		aggression.setText( String.valueOf( character.getAggression() ) );
		final JSpinner block = (JSpinner) engine.find( "block" );

		class MinMaxSpinnerModel implements SpinnerModel {
			List listeners = new ArrayList();
			int value;
			int min;
			int max;

			public MinMaxSpinnerModel( int value, int min, int max ){
				this.value = value;
				this.min = min;
				this.max = max;
			}

			public Object getNextValue(){
				return max > value ? new Integer( value + 1 ) : null;
			}

			public void addChangeListener( ChangeListener l ){
				listeners.add( l );
			}

			public void removeChangeListener( ChangeListener l ){
				listeners.remove( l );
			}

			public Object getPreviousValue(){
				return value > min ? new Integer( value - 1 ) : null;
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
		}

		block.setModel( new MinMaxSpinnerModel( findBlock( level ), 1, level.getBlocks().size() ) );
		final JSpinner map = (JSpinner) engine.find( "map" );
		map.setModel( new MinMaxSpinnerModel( character.getMap(), 0, character.getMaxMaps() ) );

		final JButton save = (JButton) engine.find( "save" );
		final JButton close = (JButton) engine.find( "close" );

		save.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				character.setName( name.getText() );
				character.setHealth( Integer.parseInt( health.getText() ) );
				character.setX( Integer.parseInt( x.getText() ) );
				character.setY( Integer.parseInt( y.getText() ) );
				character.setMap( ((Integer) map.getValue()).intValue() );
				int a = Integer.parseInt( aggression.getText() );
				if ( a != -1 ){
					character.setAggression( a );
				}
				Block b = getBlock( ((Integer) block.getValue()).intValue(), level );
				Block old = level.findBlock( character );
				if ( b != null && old != null && b != old ){
					old.removeThing( character );
					b.addThing( character );
				}

				closeProc.invoke_();
			}
		});

		close.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				closeProc.invoke_();		
			}
		});

		return (JPanel) engine.getRootComponent();
	}
}
