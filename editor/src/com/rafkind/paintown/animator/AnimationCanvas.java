package com.rafkind.paintown.animator;

/* This class models the area in the animator that contains the view area and
 * all the widgets associated with it.
 */

import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.Lambda2;

import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.DataFlavor;
import javax.activation.DataHandler;

import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.events.EventFactory;
import com.rafkind.paintown.animator.events.OffsetEvent;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.Timer;
import java.io.*;

import java.util.Iterator;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import scala.collection.JavaConversions;

import org.swixml.SwingEngine;

public abstract class AnimationCanvas extends JPanel {
    
    protected abstract JComponent makeProperties(AnimatedObject object, Animation animation, Lambda2 changeName);

    private static class ObjectBox<Data>{
        public ObjectBox(){}
        public synchronized void set(Data x){ obj = x; }
        public synchronized Data get(){ return obj; }
        private Data obj;
    }

    /* Functions that should be called when this object is being destroyed */ 
    private List<Lambda0> cleanupFunctions = new ArrayList<Lambda0>();

    public AnimationCanvas(AnimatedObject object, Animation animation, Lambda2 changeName, final Detacher detacher){
        setLayout(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();

        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;

        this.add(createPane(object, animation, changeName, detacher), constraints);
    }

    public void destroy(){
        for (Lambda0 cleanup: cleanupFunctions){
            cleanup.invoke_();
        }
    }

    private JPanel createPane(final AnimatedObject object, final Animation animation, final Lambda2 changeName, final Detacher detacher){
        final SwingEngine animEditor = new SwingEngine("animator/animation.xml");

        final JTabbedPane tabs = (JTabbedPane) animEditor.find("tabs");

        final DrawArea area = new DrawArea(object.getDrawProperties(), new Lambda0(){
            public Object invoke(){
                return null;
            }
        });

        final JButton popOut = (JButton) animEditor.find("pop-out");
        popOut.addActionListener(new AbstractAction(){
            /* detach if true, otherwise attach */
            boolean mode = true;

            /* might be called by the window being destroyed */
            private void reAttach(){
                popOut.setText("Pop out");
                detacher.attach(AnimationCanvas.this, animation.getName());
                mode = ! mode;
            }

            public void actionPerformed(ActionEvent event){
                if (mode){
                    mode = ! mode;
                    JFrame frame = detacher.detach(AnimationCanvas.this, animation.getName());
                    frame.addWindowListener(new WindowAdapter(){
                        public void windowClosing(WindowEvent event){
                            reAttach();
                        }
                    });

                    popOut.setText("Put back");
                } else {
                    reAttach();
                }

            }
        });

        JComponent properties = makeProperties(object, animation, changeName);
        if (properties != null){
            tabs.addTab("Properties", properties);
        }
        tabs.addTab("Events", makeEvents(animation, area));
        // this.save = object;

        JPanel canvas = (JPanel) animEditor.find("canvas");
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;

        canvas.add(area, constraints);

        final JSplitPane split = (JSplitPane) animEditor.find("split");
        SwingUtilities.invokeLater(new Runnable(){
            public void run(){
                /* hack to set the divider location */
                if (split.getWidth() != 0){
                    split.setDividerLocation(0.6);
                } else {
                    SwingUtilities.invokeLater(this);
                }
            }
        });
        // split.setDividerLocation(0.6);

        // SwingEngine contextEditor = new SwingEngine ( "animator/animation.xml");
        final SwingEngine contextEditor = animEditor;

        SwingEngine controlEditor = new SwingEngine("animator/controls.xml");

        JButton displayToken = (JButton) controlEditor.find( "token" );

        displayToken.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                final JDialog tempDiag = new JDialog();
                tempDiag.setSize(400,400);
                final JTextArea tempText = new JTextArea();
                final JScrollPane tempPane = new JScrollPane(tempText);
                tempDiag.add(tempPane);
                tempText.setText(animation.getToken().toString());
                tempDiag.show();
            }
        });

        JButton stopAnim = (JButton) animEditor.find("stop");
        stopAnim.addActionListener( new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                animation.stopRunning();
            }
        });

        JButton playAnim = (JButton) animEditor.find("play");
        playAnim.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                animation.startRunning();
            }
        });

        JButton previousFrame = (JButton) animEditor.find("previous-frame");
        JButton nextFrame = (JButton) animEditor.find("next-frame");

        previousFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                animation.previousFrame();
                animation.forceRedraw();
            }
        });

        nextFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                animation.nextFrame();
                animation.forceRedraw();
            }
        });

        setupTools((JComboBox) contextEditor.find("tools"), (JPanel) contextEditor.find("tool-area"), object, area, animation);

        area.animate(animation);

        return (JPanel) animEditor.getRootComponent();
    }

    private void setupTools(final JComboBox tools, final JPanel toolPane, final AnimatedObject object, final DrawArea area, final Animation animation){
        final String chooseNone = "None";
        final String chooseBackground = "Background Color";
        final String chooseGrid = "Grid";
        final String chooseOverlayAnimation = "Overlay Animation";
        final String chooseSpeedAndScale = "Speed and Scale";
        tools.addItem(chooseNone);
        tools.addItem(chooseSpeedAndScale);
        tools.addItem(chooseBackground);
        tools.addItem(chooseGrid);
        tools.addItem(chooseOverlayAnimation);

        tools.addActionListener(new AbstractAction(){
            /* TODO: If there are too many tools then create them lazily so we
             * don't spend too much time creating the animation pane on startup.
             */
            final JPanel toolNone = new JPanel();
            final JPanel toolBackground = Tools.makeBackgroundTool(object, area);
            final JPanel toolGrid = Tools.makeGridTool(area);
            final JPanel toolOverlay = makeOverlayAnimation(object, area);
            final JPanel toolSpeedAndScale = makeSpeedAndScale(animation, area);

            private JPanel getTool(String name){
                if (name.equals(chooseNone)){
                    return toolNone;
                }
                if (name.equals(chooseSpeedAndScale)){
                    return toolSpeedAndScale;
                }
                if (name.equals(chooseBackground)){
                    return toolBackground;
                }
                if (name.equals(chooseOverlayAnimation)){
                    return toolOverlay;
                }
                if (name.equals(chooseGrid)){
                    return toolGrid;
                }

                throw new RuntimeException("No such tool with name '" + name + "'");
            }

            public void actionPerformed(ActionEvent event){
                toolPane.removeAll();
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.NONE;
                constraints.anchor = GridBagConstraints.NORTHWEST;
                toolPane.add(getTool((String) tools.getSelectedItem()), constraints);
                toolPane.revalidate();
            }
        });
    }

    private JPanel makeSpeedAndScale(final Animation animation, final DrawArea area){
        final SwingEngine context = new SwingEngine("animator/tool-speed-scale.xml");
        final JLabel animationSpeed = (JLabel) context.find("speed-num");
        animationSpeed.setText("Animation speed: " + animation.getAnimationSpeed());
        final JSlider speed = (JSlider) context.find("speed");
        final double speedNumerator = 20.0;
        speed.setValue( (int) (speedNumerator / animation.getAnimationSpeed()) );
        speed.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                animation.setAnimationSpeed(speedNumerator / speed.getValue());
                animationSpeed.setText("Animation speed: " + speed.getValue() / speedNumerator);
            }
        });

        final JButton speedIncrease = (JButton) context.find("speed:increase");
        final JButton speedDecrease = (JButton) context.find("speed:decrease");

        speedIncrease.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                adjustSlider(speed, +1);
            }
        });

        speedDecrease.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                adjustSlider(speed, -1);
            }
        });

        final JLabel scaleNum = (JLabel) context.find( "scale-num" );
        scaleNum.setText( "Scale: " + area.getScale() );
        final JSlider scale = (JSlider) context.find( "scale" );
        scale.setValue( (int)(area.getScale() * 5.0) );
        scale.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                area.setScale( scale.getValue() / 5.0 );
                scaleNum.setText( "Scale: " + area.getScale() );
            }
        });

        final JButton scaleIncrease = (JButton) context.find("scale:increase");
        final JButton scaleDecrease = (JButton) context.find("scale:decrease");

        scaleIncrease.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                adjustSlider(scale, +1);
            }
        });

        scaleDecrease.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                adjustSlider(scale, -1);
            }
        });

        return (JPanel) context.getRootComponent();
    }
    
    private JPanel makeOverlayAnimation(final AnimatedObject object, final DrawArea area){
        final SwingEngine context = new SwingEngine("animator/tool-overlay-animation.xml");
        final ObjectBox<Animation> current = new ObjectBox<Animation>();
        final JComboBox animations = (JComboBox) context.find("animations");
        animations.addItem(null);
        for (Animation animation: object.getAnimations()){
            animations.addItem(animation);
        }

        final Lambda1 updateAnimations = new Lambda1(){
            public Object invoke(Object objectSelf){
                animations.removeAllItems();
                animations.addItem(null);
                for (Animation animation: object.getAnimations()){
                    animations.addItem(animation);
                }
                return null;
            }
        };

        object.addAnimationUpdate(updateAnimations);

        cleanupFunctions.add(new Lambda0(){
            public Object invoke(){
                object.removeAnimationUpdate(updateAnimations);
                return null;
            }
        });

        animations.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = (Animation) animations.getSelectedItem();
                if (current.get() != null){
                    current.get().removeDrawable(area);
                }
                current.set(animation);
                area.setOverlay(animation);
                if (animation != null){
                    animation.addDrawable(area);
                }
            }
        });

        JButton stop = (JButton) context.find("stop");
        stop.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = current.get();
                if (animation != null){
                    animation.stopRunning();
                }
            }
        });

        JButton play = (JButton) context.find("play");
        play.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = current.get();
                if (animation != null){
                    animation.startRunning();
                }
            }
        });

        final JLabel alphaText = (JLabel) context.find("alpha-text");
        final JSlider alpha = (JSlider) context.find("alpha");

        alpha.setValue((int)(area.getOverlayAlpha() * alpha.getMaximum()));
        alphaText.setText("Transparency " + area.getOverlayAlpha());
        alpha.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent change){
                area.setOverlayAlpha((double) alpha.getValue() / (double) alpha.getMaximum());
                alphaText.setText("Transparency " + area.getOverlayAlpha());
                area.repaint();
            }
        });

        JButton nextFrame = (JButton) context.find("next-frame");
        JButton previousFrame = (JButton) context.find("previous-frame");

        previousFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = current.get();
                if (animation != null){
                    animation.previousFrame();
                    animation.forceRedraw();
                }
            }
        });

        nextFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = current.get();
                if (animation != null){
                    animation.nextFrame();
                    animation.forceRedraw();
                }
            }
        });

        final JRadioButton front = (JRadioButton) context.find("front");
        final JRadioButton back = (JRadioButton) context.find("back");
        front.setActionCommand("front");
        back.setActionCommand("back");
        
        AbstractAction change = new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (event.getActionCommand().equals("front")){
                    area.setOverlayFront();
                } else {
                    area.setOverlayBehind();
                }
                area.repaint();
            }
        };

        front.addActionListener(change);
        back.addActionListener(change);

        return (JPanel) context.getRootComponent();
    }

    private JComponent makeEvents(final Animation animation, final DrawArea area){
        final SwingEngine contextEditor = new SwingEngine("animator/animation-events.xml");
        // final JScrollPane eventScroll = (JScrollPane) contextEditor.find("event-scroll");
        // eventScroll.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);

        final JList eventList = (JList) contextEditor.find("events");

        final Lambda0 updateEventList = new Lambda0(){
            private Lambda0 update = new Lambda0(){
                public Object invoke(){
                    eventList.repaint();
                    return null;
                }
            };
            public Object invoke(){
                eventList.setListData(animation.getEvents());
                for (AnimationEvent event: animation.getEvents()){
                    event.addUpdateListener(update);
                }

                return null;
            }
        };

        eventList.setDragEnabled(true);
        updateEventList.invoke_();
        eventList.setDropMode(DropMode.ON);
        eventList.setTransferHandler(new TransferHandler(){
            private int toRemove = -1;

            public boolean canImport(TransferHandler.TransferSupport support){
                // System.out.println("Can I import " + support);
                /* bleh */
                return true;
            }

            protected void exportDone(JComponent source, Transferable data, int action){
                // System.out.println("did export action was " + action);
                if (action == TransferHandler.MOVE){
                    // System.out.println("Removing event " + toRemove);
                    animation.removeEvent(toRemove);
                    updateEventList.invoke_();
                }
            }

            public int getSourceActions(JComponent component){
                // System.out.println("What are my actions? " + TransferHandler.MOVE);
                return TransferHandler.MOVE;
            }

            protected Transferable createTransferable(JComponent component){
                // System.out.println("make a transferable");
                return new Transferable(){
                    final Object index = eventList.getSelectedValue();
                    public Object getTransferData(DataFlavor flavor){
                        return index;
                    }

                    public DataFlavor[] getTransferDataFlavors(){
                        try{
                            DataFlavor[] flavors = new DataFlavor[1];
                            flavors[0] = new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType);
                            return flavors;
                        } catch (ClassNotFoundException ex){
                            System.out.println(ex);
                            return null;
                        }
                    }

                    public boolean isDataFlavorSupported(DataFlavor flavor){
                        return true;
                    }
                };
            }

            public boolean importData(TransferSupport supp) {
                // System.out.println("Try to import " + supp);
                if (!canImport(supp)) {
                    return false;
                }

                try{
                    JList.DropLocation location = (JList.DropLocation) supp.getDropLocation();
                    // System.out.println("Drop location to " + location);
                    toRemove = eventList.getSelectedIndex();
                    int index = location.getIndex();
                    if (index < toRemove){
                        toRemove += 1;
                    }
                    AnimationEvent event = (AnimationEvent) supp.getTransferable().getTransferData(new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType));
                    animation.addEvent(event, index);
                    updateEventList.invoke_();
                    eventList.setSelectedIndex(index);
                } catch (ClassNotFoundException exception){
                    System.err.println(exception);
                } catch (java.awt.datatransfer.UnsupportedFlavorException exception){
                    System.err.println(exception);
                } catch (IOException exception){
                    System.err.println(exception);
                }

                return true;
            }

        });

        final ObjectBox currentEvent = new ObjectBox();

        animation.addEventNotifier(new Lambda1(){
            public Object invoke(Object event){
                currentEvent.set(event);
                eventList.repaint();
                return null;
            }
        });

        eventList.addListSelectionListener(new ListSelectionListener(){
            public void valueChanged(ListSelectionEvent e){
                AnimationEvent event = (AnimationEvent) eventList.getSelectedValue();
                animation.stopRunning();
                animation.nextEvent(event);
                currentEvent.set(event);
            }
        });

        final ObjectBox lastEvent = new ObjectBox();
        final Lambda1 doEvent = new Lambda1(){
            final JPanel work = (JPanel) contextEditor.find("event-work");
            public Object invoke(Object _event){
                if (lastEvent.get() != null){
                    AnimationEvent last = (AnimationEvent) lastEvent.get();
                    last.destroy();
                }
                AnimationEvent event = (AnimationEvent) _event;
                lastEvent.set(event);
                JPanel editor = event.getEditor(animation, area);
                work.removeAll();
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.BOTH;
                constraints.anchor = GridBagConstraints.NORTHWEST;
                work.add(editor, constraints);
                work.revalidate();
                return null;
            }
        };

        eventList.addMouseListener( new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                if ( rightClick( e ) || e.getClickCount() == 2 ){
                    int index = eventList.locationToIndex(e.getPoint());
                    AnimationEvent event = (AnimationEvent) animation.getEvents().elementAt(index);
                    doEvent.invoke_(event);
                }
            }
        });

        eventList.setCellRenderer(new DefaultListCellRenderer(){
            public Component getListCellRendererComponent(
                JList list,
                Object value,
                int index,
                boolean isSelected,
                boolean cellHasFocus){

                setText(((AnimationEvent)value).getName());
                setBackground(isSelected ? Color.gray : Color.white);
                if (currentEvent.get() == value){
                    setForeground(Color.blue);
                } else {
                    setForeground(isSelected ? Color.white : Color.black);
                }
                return this;
            }
        });

        // Need to add events to this combobox from event factory
        // EventFactory.init();
        final JComboBox eventSelect = (JComboBox) contextEditor.find( "event-select" );
        for (Iterator it = EventFactory.getNames().iterator(); it.hasNext();){
            String event = (String) it.next();
            eventSelect.addItem(event);
        }

        final JComboBox tools = (JComboBox) contextEditor.find("tools");
        final JPanel toolPane = (JPanel) contextEditor.find("tool-area");
        final String chooseNone = "None";
        final String chooseOnionSkinning = "Onion skinning";
        final String chooseAdjustOffsets = "Adjust offsets";
        tools.addItem(chooseNone);
        tools.addItem(chooseOnionSkinning);
        tools.addItem(chooseAdjustOffsets);

        tools.addActionListener(new AbstractAction(){
            final JPanel toolNone = new JPanel();
            final JPanel toolOnionSkinning = makeOnionSkinningPanel(animation);
            final JPanel toolAdjustOffsets = makeAdjustOffsetsPanel(animation);

            private JPanel getTool(String name){
                if (name.equals(chooseNone)){
                    return toolNone;
                }
                if (name.equals(chooseOnionSkinning)){
                    return toolOnionSkinning;
                }
                if (name.equals(chooseAdjustOffsets)){
                    return toolAdjustOffsets;
                }

                throw new RuntimeException("No such tool with name '" + name + "'");
            }

            public void actionPerformed(ActionEvent event){
                toolPane.removeAll();
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.NONE;
                constraints.anchor = GridBagConstraints.NORTHWEST;
                toolPane.add(getTool((String) tools.getSelectedItem()), constraints);
                toolPane.revalidate();

            }
        });

        JButton addAllFrames = (JButton) contextEditor.find("add-frames");
        addAllFrames.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                for (String path: JavaConversions.asJavaCollection(NewAnimator.getFiles(animation.getBaseDirectory()))){
                    com.rafkind.paintown.animator.events.scala.FrameEvent frame = new com.rafkind.paintown.animator.events.scala.FrameEvent();
                    frame.setFrame(path);
                    doEvent.invoke_(frame);
                    animation.addEvent(frame);
                }
                updateEventList.invoke_();
            }
        });

        JButton eventView = (JButton) contextEditor.find("view-events");
        eventView.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                final SwingEngine engine = new SwingEngine("animator/view-events.xml");
                // final JDialog dialog = new JDialog("Event viewer");
                final JDialog dialog = new JDialog();

                final JButton close = (JButton) engine.find("close");
                close.addActionListener( new AbstractAction(){
                    public void actionPerformed(ActionEvent event ){
                        dialog.setVisible(false);
                    }
                });

                final JPanel work = (JPanel) engine.find("work");

                final Lambda1 update = new Lambda1(){
                    /* multiple requests should really just be folded
                     * into a single call to this function.
                     */
                    private synchronized void doit(){
                        int x = 0;
                        int y = 0;
                        work.removeAll();
                        double maxWidth = Math.sqrt(animation.getEvents().size());
                        for (AnimationEvent aevent : animation.getEvents()){
                            JPanel editor = aevent.getEditor(animation, area);
                            // editor.setSize(50, 50);
                            GridBagConstraints constraints = new GridBagConstraints();
                            constraints.gridx = x;
                            constraints.gridy = y;
                            constraints.weightx = 0.1;
                            constraints.weighty = 0.1;
                            constraints.fill = GridBagConstraints.BOTH;
                            // constraints.anchor = GridBagConstraints.NORTHWEST;
                            work.add(editor, constraints);
                            x += 1;
                            if (x > maxWidth){
                                y += 1;
                                x = 0;
                            }
                        }
                        work.revalidate();
                    }

                    public Object invoke(Object o){
                        SwingUtilities.invokeLater(new Runnable(){
                            public void run(){
                                doit();
                            }
                        });
                        return null;
                    }
                };

                update.invoke_(null);
                animation.addChangeUpdate(update);

                dialog.getContentPane().add((JPanel) engine.getRootComponent());
                dialog.setSize(300, 300);
                dialog.setVisible(true);
            }
        });

        JButton eventAdd = (JButton) contextEditor.find( "add-event" );
        eventAdd.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                AnimationEvent temp = EventFactory.getEvent((String)eventSelect.getSelectedItem());
                doEvent.invoke_(temp);
                int index = 0;
                if (eventList.getSelectedIndex() != -1){
                    index = animation.addEvent(temp, eventList.getSelectedIndex() + 1);
                } else {
                    index = animation.addEvent(temp);
                }
                updateEventList.invoke_();
                eventList.setSelectedIndex(index);
            }
        });

        JButton eventEdit = (JButton) contextEditor.find( "edit-event" );
        eventEdit.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if( ! animation.getEvents().isEmpty()){
                    AnimationEvent temp = (AnimationEvent) animation.getEvents().elementAt( eventList.getSelectedIndex() );
                    doEvent.invoke_(temp);
                }
            }
        });

        JButton eventRemove = (JButton) contextEditor.find("remove-event");
        AbstractAction doRemove = new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (! animation.getEvents().isEmpty()){
                    animation.removeEvent(eventList.getSelectedIndex());
                    updateEventList.invoke_();
                }
            }
        };

        eventList.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "remove-event");
        eventList.getActionMap().put("remove-event", doRemove);
        
        eventRemove.addActionListener(doRemove);

        JButton eventUp = (JButton) contextEditor.find("up-event");
        eventUp.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (! animation.getEvents().isEmpty()){
                    int index1 = eventList.getSelectedIndex()-1 < 0 ? 0 : eventList.getSelectedIndex() - 1;
                    int index2 = eventList.getSelectedIndex();
                    animation.swapEvents(index1, index2);
                    updateEventList.invoke_();
                    eventList.setSelectedIndex(index1);
                    eventList.ensureIndexIsVisible(index1);
                }
            }
        });

        JButton eventDown = (JButton) contextEditor.find("down-event");
        eventDown.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (! animation.getEvents().isEmpty()){
                    int index1 = eventList.getSelectedIndex()+1 > animation.getEvents().size() ? animation.getEvents().size() : eventList.getSelectedIndex() + 1;
                    int index2 = eventList.getSelectedIndex();
                    animation.swapEvents(index1, index2);
                    updateEventList.invoke_();
                    eventList.setSelectedIndex(index1);
                    eventList.ensureIndexIsVisible(index1);
                }
            }
        });

        final ObjectBox eventCopy = new ObjectBox();
        
        /* ctrl-c */
        eventList.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_C, 2), "copy-event");
        /* ctrl-v */
        eventList.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_V, 2), "paste-event");
        eventList.getActionMap().put("copy-event", new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (eventList.getSelectedIndex() != -1){
                    AnimationEvent what = (AnimationEvent) animation.getEvents().elementAt(eventList.getSelectedIndex());
                    eventCopy.set(what.copy());
                }
            }
        });

        eventList.getActionMap().put("paste-event", new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (eventCopy.get() != null){
                    AnimationEvent temp = ((AnimationEvent) eventCopy.get()).copy();
                    int index = 0;
                    if (eventList.getSelectedIndex() != -1){
                        index = animation.addEvent(temp, eventList.getSelectedIndex() + 1);
                    } else {
                        index = animation.addEvent(temp);
                    }
                    updateEventList.invoke_();
                    eventList.setSelectedIndex(index);
                }
            }
        });

        return (JComponent) contextEditor.getRootComponent();
    }

    private JPanel makeOnionSkinningPanel(final Animation animation){
        final SwingEngine optionsEngine = new SwingEngine("animator/onion-options.xml");
        final JSlider skins = (JSlider) optionsEngine.find("skins");
        final JLabel many = (JLabel) optionsEngine.find("how-many");
        skins.setValue(animation.getOnionSkins());

        skins.addChangeListener(new ChangeListener(){
            private String descriptive(int what){
                if (what < 0){
                    return "Showing " + what + " past frames";
                }
                if (what == 0){
                    return "Showing no frames";
                }
                if (what > 0){
                    return "Showing " + what + " future frames";
                }
                return "WTF";
            }

            public void stateChanged(ChangeEvent change){
                many.setText(descriptive(skins.getValue()));
                animation.setOnionSkins(skins.getValue());
                animation.forceRedraw();
            }
        });

        final JCheckBox onionSkinning = (JCheckBox) optionsEngine.find("onion-skinning");
        onionSkinning.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                animation.setOnionSkinning(onionSkinning.isSelected());
                animation.forceRedraw();
            }
        });

        final JRadioButton front = (JRadioButton) optionsEngine.find("front");
        final JRadioButton back = (JRadioButton) optionsEngine.find("back");
        front.setActionCommand("front");
        back.setActionCommand("back");
        
        AbstractAction change = new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (event.getActionCommand().equals("front")){
                    animation.setOnionSkinFront(true);
                } else {
                    animation.setOnionSkinFront(false);
                }
                
                animation.forceRedraw();
            }
        };

        front.addActionListener(change);
        back.addActionListener(change);

        return (JPanel) optionsEngine.getRootComponent();
    }

    private JPanel makeAdjustOffsetsPanel(final Animation animation){
        final SwingEngine optionsEngine = new SwingEngine("animator/adjust-offsets.xml");

        class OffsetAction extends AbstractAction {
            public OffsetAction(Lambda1 doOffset){
                this.doOffset = doOffset;
            }

            Lambda1 doOffset;

            public List<OffsetEvent> getOffsets(Vector<AnimationEvent> events){
                List<OffsetEvent> offsets = new ArrayList<OffsetEvent>();
                for (AnimationEvent event: events){
                    /* instanceof is justified here because we can treat
                     * events like an ADT
                     */
                    if (event instanceof OffsetEvent){
                        offsets.add((OffsetEvent) event);
                    }
                }
                return offsets;
            }

            private void updateOffsets(){
                for (OffsetEvent offset: getOffsets(animation.getEvents())){
                    try{
                        doOffset.invoke(offset);
                    } catch (Exception e){
                        System.out.println(e);
                    }
                }
            }

            public void actionPerformed(ActionEvent event){
                updateOffsets();
                animation.applyEvents();
                animation.forceRedraw();
            }
        };

        JButton left = (JButton) optionsEngine.find("left");
        JButton right = (JButton) optionsEngine.find("right");
        JButton up = (JButton) optionsEngine.find("up");
        JButton down = (JButton) optionsEngine.find("down");

        left.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() - 1);
                return null;
            }
        }));

        right.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() + 1);
                return null;
            }
        }));

        up.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() - 1);
                return null;
            }
        }));

        down.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() + 1);
                return null;
            }
        }));

        return (JPanel) optionsEngine.getRootComponent();
    }

    private void adjustSlider(JSlider slider, int much){
        slider.setValue(slider.getValue() + much);
    }

    private boolean rightClick(MouseEvent event){
        return event.getButton() == MouseEvent.BUTTON3;
    }
}
