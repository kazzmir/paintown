package com.rafkind.paintown.animator.events;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.File;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

public class FrameEvent implements AnimationEvent {
	private String frame = "";
	
	public void loadToken(Token token){
		frame = token.readString(0);
	}
	
	public void interact( Animation animation ){
		String path = Animator.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame;
		try{
			/*
			animation.setImage( MaskedImage.load( path ) );
			*/
			if ( animation.getMap() != null ){
				animation.setImage( MaskedImage.load( path, animation.getMap() ) );
			} else {
				animation.setImage( MaskedImage.load( path ) );
			}
			animation.delay();
		} catch ( Exception e ){
			e.printStackTrace();
			System.out.println( "Could not load " + path );
		}
	}
	
	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor( final Animation animation ){
		SwingEngine engine = new SwingEngine( "animator/eventframe.xml" );
		((JPanel)engine.getRootComponent()).setSize(350,270);
		JPanel canvas = (JPanel)engine.find("canvas");
		canvas.setSize(350,200);
		
		class drawArea extends JComponent {
			private BufferedImage img = null;
			public void paint( Graphics g ){
				g.setColor( new Color( 0, 0, 0 ) );
				g.fillRect( 0, 0, 640, 480 );
				if ( img != null ){
					g.drawImage( img, 125 - (img.getTileWidth()/2), 100 - (img.getTileHeight()/2), null );
				}
			}
			
			public void setImage(BufferedImage i){
				img = i;
			}
		};

		final drawArea area = new drawArea();

		area.setSize(350,200);
		area.setPreferredSize( new Dimension( 350,200 ) );
		
		canvas.add(area);
		
		final JComboBox framebox = (JComboBox) engine.find( "frame" );
		int index = 0;
		int count = -1;
		for ( Iterator it = getFiles( animation.getBaseDirectory() ).iterator(); it.hasNext(); ){
			count++;
			String _frame = (String) it.next();
			framebox.addItem(_frame);
			if ( _frame.endsWith( frame ) ){
				index = count;
			}
		}
		
		framebox.addActionListener( new ActionListener(){
			public void actionPerformed(ActionEvent actionEvent){
				frame = (String)framebox.getSelectedItem();
				try{
					area.setImage( MaskedImage.load( Animator.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame ) );
					area.repaint();
				} catch ( Exception e ){
					System.out.println("Couldn't load file: " + frame );
					e.printStackTrace();
				}
			}
		});
		
		framebox.setSelectedIndex( index );
		
		return (JPanel) engine.getRootComponent();
	}

	private List getFiles( String path ){
		File dir = Animator.dataPath( new File( path ) );
		List files = new ArrayList();
		if ( dir.isDirectory() ){
			File[] all = dir.listFiles();
			for ( int i = 0; i < all.length; i++ ){
				if ( all[ i ].getName().endsWith( ".png" ) ||
				     all[ i ].getName().endsWith( ".tga" ) ||
				     all[ i ].getName().endsWith( ".bmp" ) ){
				     	files.add( all[ i ].getName() );
					// files.add( path.replaceAll("data/","") + all[ i ].getName().replaceAll("^./","") );
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
