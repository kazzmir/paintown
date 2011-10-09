package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.RelativeFileChooser;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class ProjectileEvent implements AnimationEvent
{
	private int _at_x;
	private int _at_y;
	private String _projectile;
	private int _life;
	private double _speed_x;
	private double _speed_y;
	
	public void loadToken(Token token)
	{
		Token at = token.findToken("at");
		if(at != null)
		{
			_at_x = at.readInt(0);
			_at_y = at.readInt(1);
		}
		Token path = token.findToken("path");
		if(path != null)_projectile = path.readString(0);
		Token life = token.findToken("life");
		if(life != null)_life = life.readInt(0);
		Token y2 = token.findToken("y2");
		Token speed = token.findToken("speed");
		if(speed != null)
		{
			_speed_x = speed.readDouble(0);
			_speed_y = speed.readDouble(1);
		}
	}
	
	public void interact( Animation animation ){
		
	}

    public AnimationEvent copy(){
        ProjectileEvent event = new ProjectileEvent();
        event._at_x = _at_x;
        event._at_y = _at_y;
        event._projectile = _projectile;
        event._life = _life;
        event._speed_x = _speed_x;
        event._speed_y = _speed_y;
        return event;
    }
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		final SwingEngine engine = new SwingEngine( "animator/eventprojectile.xml" );
		((JPanel)engine.getRootComponent()).setSize(250,250);
		
		final JSpinner atxspin = (JSpinner) engine.find( "atx" );
		atxspin.setValue(new Integer(_at_x));
		atxspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_at_x = ((Integer)atxspin.getValue()).intValue();
			}
		});
		final JSpinner atyspin = (JSpinner) engine.find( "aty" );
		atyspin.setValue(new Integer(_at_y));
		atyspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_at_y = ((Integer)atyspin.getValue()).intValue();
			}
		});
		
		final JTextField profield = (JTextField) engine.find( "projectile" );
		final JButton probutton = (JButton) engine.find( "projectile-button" );
		profield.setText(_projectile);
		probutton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						profield.setText( path );
						_projectile = path;
					}
				}
			});
		
		
		final JSpinner lifespin = (JSpinner) engine.find( "life" );
		lifespin.setValue(new Integer(_life));
		lifespin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_life = ((Integer)lifespin.getValue()).intValue();
			}
		});
		
			
		final JPanel speedxpanel = (JPanel) engine.find( "speedx" );
		final JSpinner speedxspin = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		speedxpanel.add(speedxspin);
		speedxspin.setValue(new Double(_speed_x));
		speedxspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_speed_x = ((Double)speedxspin.getValue()).doubleValue();
			}
		});
		final JPanel speedypanel = (JPanel) engine.find( "speedy" );
		final JSpinner speedyspin = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		speedypanel.add(speedyspin);
		speedyspin.setValue(new Double(_speed_y));
		speedyspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_speed_y = ((Double)speedyspin.getValue()).doubleValue();
			}
		});
		
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("projectile");
		temp.addToken(new Token("projectile"));
		temp.addToken(new String[]{"at", Integer.toString(_at_x), Integer.toString(_at_y)});
		temp.addToken(new String[]{"path", _projectile});
		temp.addToken(new String[]{"life", Integer.toString(_life)});
		temp.addToken(new String[]{"speed", Double.toString(_speed_x), Double.toString(_speed_y)});
		
		return temp;
	}

        public void destroy(){
        }
}
