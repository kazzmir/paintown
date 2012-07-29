package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;
import java.awt.image.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.Timer;
import javax.swing.event.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.*;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.*;

public final class DrawArea extends JComponent {
    private int x = 50;
    private int y = 50;
    private int guideSize;
    private double scale;
    private boolean canMove = true;
    private boolean snapToGrid = false;
    /* start background as black */
    private DrawProperties drawProperties;

    private Animation currentAnimation;

    private static void drawImage(Graphics graphics, Image image, int x, int y, boolean flipX, boolean flipY){
        int width = image.getWidth(null);
        int height = image.getHeight(null);
        if (flipX && flipY){
            graphics.drawImage(image, x, y, x + width, y + height, width, height, 0, 0, null);
        } else if (flipX){
            graphics.drawImage(image, x, y, x + width, y + height, width, 0, 0, height, null);
        } else if (flipY){
            graphics.drawImage(image, x, y, x + width, y + height, 0, height, width, 0, null);  
        } else {
            graphics.drawImage(image, x, y, null);
        }
    }

    private static class OverlayImage{
        public OverlayImage(){
        }

        public boolean isBehind(){
            return ! front;
        }

        public boolean isFront(){
            return front;
        }

        public void setFront(){
            front = true;
        }

        public void setBehind(){
            front = false;
        }
        
        public void setRotation(int angle){
            this.angle = angle;
        }

        public void setOffsetX(int x){
            offsetX = x;
        }
        
        public int getOffsetX(){
            return offsetX;
        }

        public void setOffsetY(int y){
            offsetY = y;
        }
        
        public int getOffsetY(){
            return offsetY;
        }

        public void setFlipX(boolean what){
            flipX = what;
        }

        public boolean getFlipX(){
            return flipX;
        }

        public void setFlipY(boolean what){
            flipY = what;
        }

        public boolean getFlipY(){
            return flipY;
        }
        
        public double getAlpha(){
            return alpha;
        }

        public void setAlpha(double alpha){
            this.alpha = alpha;
        }

        public void draw(Graphics2D graphics, double x, double y){
            if (image != null){
                Graphics2D translucent = (Graphics2D) graphics.create();
                AlphaComposite composite = AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float)alpha);
                translucent.setComposite(composite);

                /*
                double fx = x - image.getWidth(null) / 2 + offsetX;
                double fy = y - image.getHeight(null) + offsetY;
                */
                double fx = x + offsetX;
                double fy = y + offsetY;

                /* 1. translate the image so its center is at 0, 0
                 * 2. rotate it by the angle
                 * 3. translate it by the draw position and its offset. also
                 *    take into account that the 0 is the bottom of the image
                 *    so add in another -height/2
                 */
                translucent.translate(fx, fy - image.getHeight(null) / 2);
                translucent.rotate(Math.toRadians(angle));
                translucent.translate(-image.getWidth(null) / 2, -image.getHeight(null) / 2);

                /* draw at 0, 0 to let the transforms do the work */
                drawImage(translucent, image, 0, 0, flipX, flipY);

                /*
                translucent.rotate(Math.toRadians(angle), fx, fy);
                drawImage(translucent, image, (int) fx, (int) fy, flipX, flipY);
                */

                /*
                int width = image.getWidth(null);
                int height = image.getHeight(null);
                if (flipX && flipY){
                    translucent.drawImage(image, (int) fx, (int) fy, (int) fx + width, (int) fy + height, width, height, 0, 0, null);
                } else if (flipX){
                    translucent.drawImage(image, (int) fx, (int) fy, (int) fx + width, (int) fy + height, width, 0, 0, height, null);
                } else if (flipY){
                    translucent.drawImage(image, (int) fx, (int) fy, (int) fx + width, (int) fy + height, 0, height, width, 0, null);  
                } else {
                    translucent.drawImage(image, (int) fx, (int) fy, null);
                }
                */

                translucent.dispose();
            }
        }

        public MaskedImage image;
        public boolean front = true;
        int offsetX;
        int offsetY;
        boolean flipX = false;
        boolean flipY = false;
        double alpha = 1;
        int angle = 0;
    }

    private OverlayImage overlayImage = new OverlayImage();

    private static class OverlayAnimation{
        public OverlayAnimation(){
        }

        public void setAnimation(Animation what){
            this.animation = what;
        }

        public void setAlpha(double level){
            this.alpha = level;
            alpha = alpha;
            if (alpha < 0){
                alpha = 0;
            }
            if (alpha > 1){
                alpha = 1;
            }
        }

        public double getAlpha(){
            return alpha;
        }

        public void setFlipX(boolean what){
            flipX = what;
        }
        
        public void setFlipY(boolean what){
            flipY = what;
        }
        
        public void setOffsetX(int x){
            this.offsetX = x;
        }

        public int getOffsetX(){
            return this.offsetX;
        }
        
        public void setOffsetY(int x){
            this.offsetY = x;
        }
        
        public int getOffsetY(){
            return this.offsetY;
        }

        public void draw(Graphics graphics, int x, int y, double scale){
            if (animation != null){
                Graphics2D translucent = (Graphics2D) graphics.create();
                AlphaComposite alpha = AlphaComposite.getInstance(AlphaComposite.SRC_OVER, (float)this.alpha);
                translucent.setComposite(alpha);
                AffineTransform flipper = new AffineTransform();
                double xscale = 1;
                double yscale = 1;
                if (flipX){
                    xscale = -1;
                }
                if (flipY){
                    yscale = -1;
                }

                flipper.scale(xscale * scale, yscale * scale);
                flipper.translate(xscale * (x + offsetX), yscale * (y + offsetY));

                translucent.setTransform(flipper);
                animation.draw(translucent, 0, 0);
                translucent.dispose();
            }
        }

        private Animation animation;
        private double alpha = 1;
        private boolean flipX = false;
        private boolean flipY = false;
        private int offsetX = 0;
        private int offsetY = 0;
    }

    private OverlayAnimation overlayAnimation = new OverlayAnimation();

    /* true for behind, false for in front */
    private boolean overlayBehind = true;
    private boolean resizedOnce = false;

    private java.util.List<String> helpText = new ArrayList<String>();
    private java.util.List<Lambda0> scaleListeners = new ArrayList<Lambda0>();

    public DrawArea(final Lambda0 loader){
        this(new DrawProperties(), loader);
    }

    public DrawArea(DrawProperties properties, final Lambda0 loader){
        setFocusable(true);
        currentAnimation = null;
        this.drawProperties = properties;

        scale = 1.0;

        this.addComponentListener(new ComponentAdapter(){
            public void componentResized(ComponentEvent event){
                if (resizedOnce == false){
                    resizedOnce = true;
                    if (currentAnimation != null && currentAnimation.hasImage()){
                        double width = currentAnimation.getWidth();
                        double height = currentAnimation.getHeight();
                        double myWidth = getWidth();
                        double myHeight = getHeight();
                        if (myWidth / width < myHeight / height){
                            setScale(myWidth / width * 0.5);
                        } else {
                            setScale(myWidth / height * 0.5);
                        }
                        updateScaleListeners();
                    }
                    setCenterX((int)(getWidth() / 2.0 / getScale()));
                    setCenterY((int)(getHeight() / 2.0 / getScale()) + 10);
                }
            }
        });

        this.addMouseListener(new MouseListener(){
            public void mouseClicked(MouseEvent e){
                requestFocusInWindow();
            }

            public void mousePressed(MouseEvent event){
                requestFocusInWindow();
            }

            public void mouseEntered(MouseEvent event){
            }

            public void mouseExited(MouseEvent event){
            }

            public void mouseReleased(MouseEvent event){
            }
        });

        this.addMouseMotionListener( new MouseMotionAdapter(){
            public void mouseDragged(MouseEvent event){
                requestFocusInWindow();
                if (canMove){
                    if (DrawArea.this.isSnapToGrid()){
                        double whereX = event.getX() / getScale();
                        double whereY = event.getY() / getScale();
                        setCenterX((int) closestSnapX(whereX));
                        setCenterY((int) closestSnapY(whereY));
                    } else {
                        setCenterX((int)(event.getX() / getScale()));
                        setCenterY((int)(event.getY() / getScale()));
                    }
                    DrawArea.this.repaint();
                }
            }
        });

        class PopupHack{
            private Popup box;
            private int counter;
            public PopupHack(JComponent insides){
                Point here = DrawArea.this.getLocation();
                SwingUtilities.convertPointToScreen(here, DrawArea.this);
                box = PopupFactory.getSharedInstance().getPopup(DrawArea.this, insides, (int) here.getX(), (int) here.getY());
                box.show();
                counter = 10;
            }

            public void hide(){
                box.hide();
            }

            public boolean decrement(){
                counter -= 1;
                return counter <= 0;
            }

            public void pressed(){
                counter = 10;
            }
        }

        final PopupHack[] box = new PopupHack[1];

        this.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_Q, 0, false ), "press" );
        this.getActionMap().put( "press", new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                boolean ok = false;
                synchronized(box){
                    ok = box[0] == null;
                }
                if (ok){
                    /*
                       System.out.println("Pressed a key");
                       JButton button = new JButton("hello!");
                       button.setSize(100,100);
                       */
                    JComponent input = (JComponent) loader.invoke_();
                    if (input == null){
                        return;
                    }
                    synchronized(box){
                        box[0] = new PopupHack(input);
                    }
                    final Timer t = new Timer(500, new ActionListener(){
                        public void actionPerformed( ActionEvent event ){
                        }
                    });
                    t.addActionListener(new ActionListener(){
                        public void actionPerformed(ActionEvent event){
                            synchronized(box){
                                if ( box[0] != null && box[0].decrement() ){
                                    box[0].hide();
                                    box[0] = null;
                                    t.stop();
                                }
                            }
                        }
                    });
                    t.start();
                } else {
                    synchronized(box){
                        box[0].pressed();
                    }
                }
            }
        });

        this.addKeyListener(new KeyListener(){
            private boolean isControl(KeyEvent event){
                return event.getKeyCode() == KeyEvent.VK_CONTROL;
            }

            public void keyPressed(KeyEvent event){
                if (isControl(event)){
                    DrawArea.this.enableSnapToGrid();
                }
            }

            public void keyReleased(KeyEvent event){
                if (isControl(event)){
                    DrawArea.this.disableSnapToGrid();
                }
            }

            public void keyTyped(KeyEvent event){
            }
        });

        /*
           this.addKeyListener(new KeyListener(){
           private Popup box;
           public void keyPressed(KeyEvent e){
           if ( box == null ){
           System.out.println("Pressed a key");
           jbutton button = new jbutton("hello!");
           button.setsize(100,100);
           box = popupfactory.getsharedinstance().getpopup(drawarea.this, button, getx(), gety());
           box.show();
           }
           }

           public void keyReleased(KeyEvent e){
           if ( box != null ){
           System.out.println("Key released");
           box.hide();
           box = null;
           }
           }

           public void keyTyped(KeyEvent e){
           }
           });
           */
    }

    private void updateScaleListeners(){
        for (Lambda0 update: scaleListeners){
            update.invoke_();
        }
    }

    public void addScaleListener(Lambda0 update){
        scaleListeners.add(update);
    }

    public void setOverlayImageRotation(int angle){
        overlayImage.setRotation(angle);
    }

    public void setOverlayImage(MaskedImage image){
        overlayImage.image = image;
    }
        
    public void setOverlayImageFlipX(boolean what){
        overlayImage.setFlipX(what);
    }
    
    public void setOverlayImageFlipY(boolean what){
        overlayImage.setFlipY(what);
    }
    
    public void setOverlayImageFront(){
        overlayImage.setFront();
    }
    
    public void setOverlayImageBehind(){
        overlayImage.setBehind();
    }

    public void setOverlayImageAlpha(double alpha){
        overlayImage.setAlpha(alpha);
    }
    
    public double getOverlayImageAlpha(){
        return overlayImage.getAlpha();
    }
    
    public void setOverlayImageOffsetX(int x){
        overlayImage.setOffsetX(x);
    }

    public int getOverlayImageOffsetY(){
        return overlayImage.getOffsetY();
    }
    
    public int getOverlayImageOffsetX(){
        return overlayImage.getOffsetX();
    }
    
    public void setOverlayImageOffsetY(int y){
        overlayImage.setOffsetY(y);
    }

    private double getGuideRatio(){
        if (guideSize > 0){
            return 10 * getMaxGuideSize() / guideSize;
        }
        return 1;
    }

    public double closestSnapX(double x){
        return getGuideRatio() * Math.round(x / getGuideRatio());
    }

    public double closestSnapY(double y){
        return getGuideRatio() * Math.round(y / getGuideRatio());
    }

    public boolean isSnapToGrid(){
        return snapToGrid;
    }

    public void enableSnapToGrid(){
        // System.out.println("Enable snap to grid");
        snapToGrid = true;
    }

    public void disableSnapToGrid(){
        // System.out.println("Disable snap to grid");
        snapToGrid = false;
    }

    public double getScale(){
        return scale;
    }

    public void setScale(double x){
        if (x < 0.1){
            x = 0.1;
        }
        if (x > 4){
            x = 4;
        }
        scale = x;
        repaint();
    }

    public double getMaxGuideSize(){
        return 10;
    }

    public int getGuideSize(){
        return guideSize;
    }

    public void setGuideSize(int size){
        guideSize = size;
    }


    /*
       public Dimension getPreferredSize(){
       return new Dimension(800,600);
       }
       */

    /*
    public void setBackgroundColor(Color color){
        backgroundColor = color;
    }

    public Color backgroundColor(){
        return backgroundColor;
    }
    */

    public void setOverlayBehind(){
        overlayBehind = true;
    }

    public void setOverlayFront(){
        overlayBehind = false;
    }

    /* set opaqueness of the overlay image.
     * 1 - opaque
     * 0 - translucent
     * anything in between will be partially transparent
     */
    public void setOverlayAlpha(double alpha){
        overlayAnimation.setAlpha(alpha);
    }

    public void setOverlayAnimationOffsetX(int x){
        overlayAnimation.setOffsetX(x);
    }

    public int getOverlayAnimationOffsetX(){
        return overlayAnimation.getOffsetX();
    }
    
    public void setOverlayAnimationOffsetY(int y){
        overlayAnimation.setOffsetY(y);
    }

    public int getOverlayAnimationOffsetY(){
        return overlayAnimation.getOffsetY();
    }

    public void setOverlayAnimationFlipX(boolean what){
        overlayAnimation.setFlipX(what);
    }
    
    public void setOverlayAnimationFlipY(boolean what){
        overlayAnimation.setFlipY(what);
    }

    public double getOverlayAlpha(){
        return overlayAnimation.getAlpha();
    }

    public void setOverlay(Animation animation){
        this.overlayAnimation.setAnimation(animation);
    }
    
    public Color backgroundColor(){
        return drawProperties.getBackgroundColor();
    }

    private Color oppositeColor(Color what){
        /* don't bother fooling with HSB. the code here looks fine */
        return new Color(255 - what.getRed(),
                         255 - what.getGreen(),
                         255 - what.getBlue());
    }

    private void drawGrid(Graphics2D graphics){
        if (getGuideSize() > 0){
            graphics.setColor(oppositeColor(backgroundColor()));
            for (double x = 0; x < getWidth(); x += getGuideRatio()){
                graphics.drawLine((int) x, 0, (int) x, getHeight());
            }
            for (double y = 0; y < getHeight(); y += getGuideRatio()){
                graphics.drawLine(0, (int) y, getWidth(), (int) y);
            }
        }
    }

    private void drawOverlay(Graphics2D g, int x, int y){
        overlayAnimation.draw(g, x, y, getScale());
    }

    public void addHelpText(String... lines){
        for (String line: lines){
            this.helpText.add(line);
        }
    }

    public void removeHelpText(){
        this.helpText = new ArrayList<String>();
    }

    public void drawHelpText(Graphics2D graphics){
        graphics.setColor(new Color(255, 255, 255));
        int height = graphics.getFontMetrics(graphics.getFont()).getHeight();
        int y = height + 2;
        for (String line: helpText){
            graphics.drawString(line, 2, y);
            y += height + 2;
        }
    }

    public void paintComponent(Graphics g){
        Graphics2D g2d = (Graphics2D) g;
        g2d.scale(getScale(), getScale());

        g.setColor(backgroundColor());
        g.fillRect(0, 0, (int) (getWidth() / getScale()), (int) (getHeight() / getScale()));
        drawGrid(g2d);
        g.setColor(new Color(255, 255, 0));
        g.drawLine(0, y, (int) (getWidth() / getScale()), y);
        g.drawLine(x, 0, x, (int) (getHeight() / getScale()));

        if (overlayImage.isBehind()){
            overlayImage.draw(g2d, x, y);
        }

        if (overlayBehind){
            drawOverlay(g2d, x, y);
        }

        if (currentAnimation != null){
            Graphics2D blah = (Graphics2D) g2d.create();
            AffineTransform transform = new AffineTransform();
            transform.scale(getScale(), getScale());
            transform.translate(x, y);
            blah.setTransform(transform);
            currentAnimation.draw(blah, 0, 0);
            // currentAnimation.draw(g, x, y);
            blah.dispose();
        }

        if (! overlayBehind){
            drawOverlay(g2d, x, y);
        }
        
        if (overlayImage.isFront()){
            overlayImage.draw(g2d, x, y);
        }

        /* Undo the scale to get back to 1x1 */
        g2d.scale(1 / getScale(), 1 / getScale());
        drawHelpText(g2d);
    }

    public void setCenterX(int x){
        this.x = x;
    }

    public void setCenterY(int y){
        this.y = y;
    }

    public int getCenterX(){
        return x;
    }

    public int getCenterY(){
        return y;
    }

    public void enableMovement(){
        this.canMove = true;
    }
    
    public void disableMovement(){
        this.canMove = false;
    }

    public void animate(Animation animation){
        unanimate();
        currentAnimation = animation;
        currentAnimation.addDrawable(this);
    }

    public void unanimate(){
        if (currentAnimation != null){
            currentAnimation.removeDrawable(this);
        }
    }
}
