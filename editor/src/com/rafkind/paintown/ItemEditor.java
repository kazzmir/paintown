package com.rafkind.paintown;

import java.awt.event.*;
import javax.swing.*;

import com.rafkind.paintown.level.Item;
import com.rafkind.paintown.level.Level;

import org.swixml.SwingEngine;

public class ItemEditor implements PropertyEditor {
	
	private Item item;

	public ItemEditor( Item i ){
		item = i;
	}

	public JComponent createPane( final Level level, final Lambda0 closeProc ){
		final SwingEngine engine = new SwingEngine( "item.xml" );

		final JButton save = (JButton) engine.find( "save" );
		final JButton close = (JButton) engine.find( "close" );

		final JTextField x = (JTextField) engine.find( "x" );
		x.setText( String.valueOf( item.getX() ) );
		final JTextField y = (JTextField) engine.find( "y" );
		y.setText( String.valueOf( item.getY() ) );

		final Lambda1 update = new Lambda1(){
			public Object invoke( Object c ){
				Item item = (Item) c;
				x.setText( String.valueOf( item.getX() ) );
				y.setText( String.valueOf( item.getY() ) );
				return null;
			}
		};

		item.addListener( update );

		save.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				/*
				int xInt = Integer.parseInt( x.getText() );
				int yInt = Integer.parseInt( y.getText() );
				character.setName( name.getText() );
				character.setHealth( Integer.parseInt( health.getText() ) );
				character.setX( xInt );
				character.setY( yInt );
				character.setMap( ((Integer) map.getValue()).intValue() );
				int a = Integer.parseInt( aggression.getText() );
				character.setAggression( a );
				Block b = getBlock( ((Integer) block.getValue()).intValue(), level );
				Block old = level.findBlock( character );
				if ( b != null && old != null && b != old ){
					old.removeThing( character );
					b.addThing( character );
				}
				*/

				item.removeListener( update );
				closeProc.invoke_();
			}
		});

		close.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				item.removeListener( update );
				closeProc.invoke_();		
			}
		});

		return (JPanel) engine.getRootComponent();
	}
}
