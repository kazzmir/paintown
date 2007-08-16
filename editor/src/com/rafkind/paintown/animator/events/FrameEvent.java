package com.rafkind.paintown.animator.events;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.File;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

public class FrameEvent implements AnimationEvent
{
	private String frame = "";
	
	public void loadToken(Token token)
	{
		Token parent = token.getParent();
		Token basedir = parent.findToken("basedir");
		if(basedir != null){
			frame = basedir.readString(0) + token.readString(0);
		} else {
			frame = token.readString(0);
		}
	}
	
	public void interact( Animation animation ){
		try{
			animation.setImage(MaskedImage.load(Animator.getDataPath() + "/" + frame));
			animation.delay();
		} catch ( Exception e ){
			e.printStackTrace();
			System.out.println("Problem! --> " + Animator.getDataPath() + "/" + frame);
		}
	}
	
	public String getName(){
		return getToken().toString();
	}
	
	public JDialog getEditor( Animation animation ){
		SwingEngine engine = new SwingEngine( "animator/eventframe.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JComboBox framebox = (JComboBox) engine.find( "frame" );
		for ( Iterator it = getFiles( animation.getBaseDirectory() ).iterator(); it.hasNext(); ){
			String frame = (String) it.next();
			framebox.addItem(frame);
		}
		
		framebox.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent actionEvent){
				frame = (String)framebox.getSelectedItem();
			}
		});
		return (JDialog)engine.getRootComponent();
	}

	private List getFiles( String path ){
		File dir = Animator.dataPath( new File( path ) );
		List files = new ArrayList();
		if ( dir.isDirectory() ){
			File[] all = dir.listFiles();
			for ( int i = 0; i < all.length; i++ ){
				if ( all[ i ].getName().endsWith( ".png" ) ||
				     all[ i ].getName().endsWith( ".bmp" ) ){
					files.add( path.replaceAll("data/","") + all[ i ].getName().replaceAll("^./","") );
				}
			}
		}
		return files;
	}
	
	public Token getToken(){
		Token temp = new Token("frame");
		temp.addToken( new Token( "frame" ) );
		temp.addToken( new Token( frame ) );
		return temp;
	}
}
