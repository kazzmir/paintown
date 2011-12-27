package com.rafkind.paintown.level;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.Iterator;

import com.rafkind.paintown.level.objects.Item;
import com.rafkind.paintown.level.objects.Level;
import com.rafkind.paintown.level.objects.Block;
import com.rafkind.paintown.level.objects.Stimulation;
import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.MinMaxSpinnerModel;

import org.swixml.SwingEngine;

public class ItemEditor implements PropertyEditor {
	
	private Item item;

	public ItemEditor(Item i){
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
        final JSpinner id = (JSpinner) engine.find("id");
		final JTextField path = (JTextField) engine.find( "path" );
		path.setText(item.getPath());
		path.setEditable(false);
        
        id.setValue(item.getId());
		
        block.setModel(new MinMaxSpinnerModel(findBlock(level), 1, level.getBlocks().size()));

		final JSpinner healthSpinner = (JSpinner) engine.find("health-spinner");
		final JRadioButton healthRadio = (JRadioButton) engine.find("health");
        final JRadioButton invincibilityRadio = (JRadioButton) engine.find("invincibility");
		final JSpinner invincibilitySpinner = (JSpinner) engine.find("invincibility-spinner");
        final JRadioButton speedRadio = (JRadioButton) engine.find("speed");
		final JSpinner speedSpinner = (JSpinner) engine.find("speed-spinner");
		final JSpinner speedDurationSpinner = (JSpinner) engine.find("speed-duration");

        /* get the stimulation from the existing item or make a new one */
		final Stimulation.HealthStimulation healthStimulation = item.getStimulation() != null && item.getStimulation() instanceof Stimulation.HealthStimulation ? new Stimulation.HealthStimulation((Stimulation.HealthStimulation) item.getStimulation()) : new Stimulation.HealthStimulation();

		if (item.getStimulation() instanceof Stimulation.HealthStimulation){
			healthRadio.setSelected(true);
			healthSpinner.setEnabled(true);
            invincibilitySpinner.setEnabled(false);
            speedSpinner.setEnabled(false);
            speedDurationSpinner.setEnabled(false);
		}
        
        if (item.getStimulation() instanceof Stimulation.InvincibilityStimulation){
			invincibilityRadio.setSelected(true);
            invincibilitySpinner.setEnabled(true);
			healthSpinner.setEnabled(false);
            speedSpinner.setEnabled(false);
            speedDurationSpinner.setEnabled(false);
        }
 
        if (item.getStimulation() instanceof Stimulation.SpeedStimulation){
			speedRadio.setSelected(true);
            speedDurationSpinner.setEnabled(true);
            speedSpinner.setEnabled(true);
			healthSpinner.setEnabled(false);
            invincibilitySpinner.setEnabled(false);
        }

		healthRadio.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent event){
				healthSpinner.setEnabled(true);
                invincibilitySpinner.setEnabled(false);
                speedSpinner.setEnabled(false);
                speedDurationSpinner.setEnabled(false);
			}
		});

		healthSpinner.setValue(new Integer(healthStimulation.getHealth()));
		healthSpinner.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e){
				JSpinner spinner = healthSpinner;
				Integer i = (Integer) spinner.getValue();
				healthStimulation.setHealth(i.intValue());
			}
		});

		final Stimulation.InvincibilityStimulation invincibilityStimulation = item.getStimulation() != null && item.getStimulation() instanceof Stimulation.InvincibilityStimulation ? new Stimulation.InvincibilityStimulation((Stimulation.InvincibilityStimulation) item.getStimulation()) : new Stimulation.InvincibilityStimulation();

		invincibilityRadio.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent event){
				invincibilitySpinner.setEnabled(true);
				healthSpinner.setEnabled(false);
                speedSpinner.setEnabled(false);
                speedDurationSpinner.setEnabled(false);
			}
		});

        invincibilitySpinner.setValue(new Integer(invincibilityStimulation.getDuration()));
		invincibilitySpinner.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e){
				JSpinner spinner = invincibilitySpinner;
				Integer i = (Integer) spinner.getValue();
				invincibilityStimulation.setDuration(i.intValue());
			}
		});
		
        final Stimulation.SpeedStimulation speedStimulation = item.getStimulation() != null && item.getStimulation() instanceof Stimulation.SpeedStimulation ? new Stimulation.SpeedStimulation((Stimulation.SpeedStimulation) item.getStimulation()) : new Stimulation.SpeedStimulation();

		speedRadio.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent event){
				healthSpinner.setEnabled(false);
                invincibilitySpinner.setEnabled(false);
                speedSpinner.setEnabled(true);
                speedDurationSpinner.setEnabled(true);
			}
		});

        speedSpinner.setModel(new SpinnerNumberModel(speedStimulation.getBoost(), 0.0, 100.0, 0.1));
		speedSpinner.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e){
				JSpinner spinner = speedSpinner;
				Number i = (Number) spinner.getValue();
				speedStimulation.setBoost(i.doubleValue());
			}
		});

        speedDurationSpinner.setValue(new Integer(speedStimulation.getTicks()));
		speedDurationSpinner.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent e){
				JSpinner spinner = speedDurationSpinner;
				Integer i = (Integer) spinner.getValue();
				speedStimulation.setTicks(i.intValue());
			}
		});

		final JRadioButton noneRadio = (JRadioButton) engine.find( "none" );
		noneRadio.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				healthSpinner.setEnabled(false);
                invincibilitySpinner.setEnabled(false);
                speedSpinner.setEnabled(false);
			}
		});
		
		final Lambda1 update = new Lambda1(){
			public Object invoke(Object c){
				Item item = (Item) c;
				x.setText(String.valueOf(item.getX()));
				y.setText(String.valueOf(item.getY()));
				return null;
			}
		};

		item.addListener(update);

		save.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				int xInt = Integer.parseInt( x.getText() );
				int yInt = Integer.parseInt( y.getText() );
				item.setName( name.getText() );
				item.setX( xInt );
				item.setY( yInt );
                item.setId(((Integer) id.getValue()).intValue());
				Block b = getBlock( ((Integer) block.getValue()).intValue(), level );
				Block old = level.findBlock( item );
				if (b != null && old != null && b != old){
					old.removeThing(item);
					b.addThing(item);
				}

				item.removeListener( update );
				closeProc.invoke_();

				if (noneRadio.isSelected()){
					item.setStimulation(null);
				} else if (healthRadio.isSelected()){
					item.setStimulation(healthStimulation);
				} else if (invincibilityRadio.isSelected()){
                    item.setStimulation(invincibilityStimulation);
                } else if (speedRadio.isSelected()){
                    item.setStimulation(speedStimulation);
                }
			}
		});

		close.addActionListener(new AbstractAction(){
			public void actionPerformed(ActionEvent event){
				item.removeListener(update);
				closeProc.invoke_();		
			}
		});

		return (JPanel) engine.getRootComponent();
	}
}
