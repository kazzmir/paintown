package com.rafkind.paintown;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.Iterator;

import com.rafkind.paintown.level.Item;
import com.rafkind.paintown.level.Level;
import com.rafkind.paintown.level.Block;
import com.rafkind.paintown.level.Stimulation;

import org.swixml.SwingEngine;

public class ItemEditor implements PropertyEditor {
	
	private Item item;

	public ItemEditor( Item i ){
		item = i;
	}

	private int findBlock( Level level ){
		int i = 1;
		for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.hasThing( item ) ){
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
		final SwingEngine engine = new SwingEngine( "item.xml" );

		final JButton save = (JButton) engine.find( "save" );
		final JButton close = (JButton) engine.find( "close" );

		final JTextField name = (JTextField) engine.find( "name" );
		name.setText( item.getName() );
		final JTextField x = (JTextField) engine.find( "x" );
		x.setText( String.valueOf( item.getX() ) );
		final JTextField y = (JTextField) engine.find( "y" );
		y.setText( String.valueOf( item.getY() ) );
		final JSpinner block = (JSpinner) engine.find( "block" );
		final JTextField path = (JTextField) engine.find( "path" );
		path.setText( item.getPath() );
		path.setEditable( false );

		final JLabel healthLabel = (JLabel) engine.find( "health-label" );
		final JSpinner healthSpinner = (JSpinner) engine.find( "health-spinner" );
		final JRadioButton healthRadio = (JRadioButton) engine.find( "health" );

		final Stimulation.HealthStimulation healthStimulation = item.getStimulation() != null && item.getStimulation() instanceof Stimulation.HealthStimulation ? new Stimulation.HealthStimulation( item.getStimulation() ) : new Stimulation.HealthStimulation();

		if ( item.getStimulation() instanceof Stimulation.HealthStimulation ){
			healthRadio.setSelected( true );
			healthLabel.setEnabled( true );
			healthSpinner.setEnabled( true );
		}

		healthRadio.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				healthLabel.setEnabled( true );
				healthSpinner.setEnabled( true );
			}
		});

		healthSpinner.setValue( new Integer( healthStimulation.getHealth() ) );
		healthSpinner.addChangeListener( new ChangeListener(){
			public void stateChanged( ChangeEvent e ){
				JSpinner spinner = healthSpinner;
				Integer i = (Integer) spinner.getValue();
				healthStimulation.setHealth( i.intValue() );
			}
		});

		final JRadioButton noneRadio = (JRadioButton) engine.find( "none" );
		noneRadio.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				healthLabel.setEnabled( false );
				healthSpinner.setEnabled( false );
			}
		});
		
		block.setModel( new MinMaxSpinnerModel( findBlock( level ), 1, level.getBlocks().size() ) );

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
				int xInt = Integer.parseInt( x.getText() );
				int yInt = Integer.parseInt( y.getText() );
				item.setName( name.getText() );
				item.setX( xInt );
				item.setY( yInt );
				Block b = getBlock( ((Integer) block.getValue()).intValue(), level );
				Block old = level.findBlock( item );
				if ( b != null && old != null && b != old ){
					old.removeThing( item );
					b.addThing( item );
				}

				item.removeListener( update );
				closeProc.invoke_();

				if ( noneRadio.isSelected() ){
					item.setStimulation( null );
				} else if ( healthRadio.isSelected() ){
					item.setStimulation( healthStimulation );
				}
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
