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
import com.rafkind.paintown.Data;
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

    public void interact(final Animation animation){
        String path = Data.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame;
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

    public AnimationEvent copy(){
        return null;
    }

    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor( final Animation animation, final DrawArea area2){
        SwingEngine engine = new SwingEngine( "animator/eventframe.xml" );
        // ((JPanel)engine.getRootComponent()).setSize(350,270);
        // JPanel canvas = (JPanel)engine.find("canvas");
        final JPanel canvas = (JPanel) engine.getRootComponent();

        class drawArea extends JComponent {
            private BufferedImage img = null;
            public void paint( Graphics g ){
                g.setColor( new Color( 0, 0, 0 ) );
                // g.fillRect( 0, 0, 640, 480 );
                g.fillRect(1, 1, getWidth() - 1, getHeight() - 1);
                if (img != null){
                    // g.drawImage( img, 125 - (img.getTileWidth()/2), 100 - (img.getTileHeight()/2), null );
                    Graphics2D g2d = (Graphics2D) g;
                    double scale = Math.min((getWidth() - 5.0) / img.getTileWidth(), (getHeight() - 5.0) / img.getTileHeight());
                    g2d.scale(scale, scale);
                    // g.drawImage(img, (int)(getWidth() / 2 - (img.getTileWidth()*scale/2)), (int)(getHeight() / 2 - img.getTileHeight()*scale/2), null);
                    // g.drawImage(img, (int)(getWidth() / 2 - (img.getTileWidth()*scale/2)), (int)(getHeight() / 2 - img.getTileHeight()*scale/2), null);
                    g.drawImage(img, (int)(getWidth() / 2 - (img.getWidth(null)*scale/2)), (int)(getHeight() / 2 - img.getHeight(null)*scale/2), null);
                    // g.drawImage(img, (int) ((getWidth() / 2 - (img.getTileWidth()/2)) * scale), (int)((getHeight() / 2 - (img.getTileHeight()/2)) * scale), null);
                }
            }

            public void setImage(BufferedImage i){
                img = i;
            }
        };

        final drawArea area = new drawArea();

        /*
           area.setSize(350,200);
           area.setPreferredSize( new Dimension( 350,200 ) );
           */
        area.setPreferredSize(new Dimension(100,100));

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;
        canvas.add(area, constraints);

        // canvas.add(area);

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
                    area.setImage( MaskedImage.load( Data.getDataPath() + "/" + animation.getBaseDirectory() + "/" + frame ) );
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
        /* use a FileFilter here */
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

    public void destroy(){
    }
}
