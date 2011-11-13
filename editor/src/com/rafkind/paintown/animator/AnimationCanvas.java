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

    private class ObjectBox{
        public ObjectBox(){}
        public synchronized void set( Object x ){ obj = x; }
        public synchronized Object get(){ return obj; }
        private Object obj;
    }

    public AnimationCanvas(AnimatedObject object, Animation animation, Lambda2 changeName){
        setLayout(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();

        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;

        this.add(createPane(object, animation, changeName), constraints);
    }

    private JPanel createPane(final AnimatedObject object, final Animation animation, final Lambda2 changeName){
        final SwingEngine animEditor = new SwingEngine("animator/animation.xml");

        final JTabbedPane tabs = (JTabbedPane) animEditor.find("tabs");

        final DrawArea area = new DrawArea(object.getDrawProperties(), new Lambda0(){
            public Object invoke(){
                /*
                   if ( eventList.getSelectedIndex() != -1 ){
                   AnimationEvent event = (AnimationEvent) eventList.getSelectedValue();
                   return event.getEditor(animation, );
                   } else {
                   return null;
                   }
                   */

                return null;
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

        JPanel controls = (JPanel) animEditor.find( "controls" );

        JButton displayToken = (JButton) controlEditor.find( "token" );

        displayToken.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                final JDialog tempDiag = new JDialog();
                tempDiag.setSize(400,400);
                final JTextArea tempText = new JTextArea();
                final JScrollPane tempPane = new JScrollPane(tempText);
                tempDiag.add(tempPane);
                tempText.setText( animation.getToken().toString());
                tempDiag.show();
            }
        });

        JButton stopAnim = (JButton) animEditor.find( "stop" );
        stopAnim.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                animation.stopRunning();
            }
        });

        JButton playAnim = (JButton) animEditor.find( "play" );
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

        final JComboBox tools = (JComboBox) contextEditor.find("tools");
        final JPanel toolPane = (JPanel) contextEditor.find("tool-area");
        final String chooseNone = "None";
        final String chooseBackground = "Background Color";
        final String chooseGrid = "Grid";
        tools.addItem(chooseNone);
        tools.addItem(chooseBackground);
        tools.addItem(chooseGrid);

        tools.addActionListener(new AbstractAction(){
            final JPanel toolNone = new JPanel();
            final JPanel toolBackground = makeBackgroundTool(object, area, animation);
            final JPanel toolGrid = makeGridTool(area, animation);

            private JPanel getTool(String name){
                if (name.equals(chooseNone)){
                    return toolNone;
                }
                if (name.equals(chooseBackground)){
                    return toolBackground;
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

        final JLabel animationSpeed = (JLabel) contextEditor.find("speed-num");
        animationSpeed.setText( "Animation speed: " + animation.getAnimationSpeed() );
        final JSlider speed = (JSlider) contextEditor.find("speed");
        final double speedNumerator = 20.0;
        speed.setValue( (int) (speedNumerator / animation.getAnimationSpeed()) );
        speed.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                animation.setAnimationSpeed(speedNumerator / speed.getValue());
                animationSpeed.setText("Animation speed: " + speed.getValue() / speedNumerator);
            }
        });

        final JButton speedIncrease = (JButton) contextEditor.find("speed:increase");
        final JButton speedDecrease = (JButton) contextEditor.find("speed:decrease");

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

        // controls.add((JComponent)controlEditor.getRootComponent());


        final JLabel scaleNum = (JLabel) animEditor.find( "scale-num" );
        scaleNum.setText( "Scale: " + area.getScale() );
        final JSlider scale = (JSlider) animEditor.find( "scale" );
        scale.setValue( (int)(area.getScale() * 5.0) );
        scale.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                area.setScale( scale.getValue() / 5.0 );
                scaleNum.setText( "Scale: " + area.getScale() );
            }
        });

        final JButton scaleIncrease = (JButton) animEditor.find("scale:increase");
        final JButton scaleDecrease = (JButton) animEditor.find("scale:decrease");

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

        area.animate(animation);

        return (JPanel) animEditor.getRootComponent();
    }

    private JPanel makeGridTool(final DrawArea area, final Animation animation){
        final SwingEngine context = new SwingEngine("animator/animation-tools.xml");
        final JSlider guide = (JSlider) context.find("guide");
        guide.setValue(area.getGuideSize());
        guide.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent change){
                area.setGuideSize(guide.getValue());
                animation.forceRedraw();
            }
        });
        return (JPanel) context.find("grid");
    }

    private JPanel makeBackgroundTool(final AnimatedObject character, final DrawArea area, final Animation animation){
        JPanel panel = new JPanel();
        final JColorChooser color = new JColorChooser(area.backgroundColor());
        color.setPreviewPanel(new JPanel());
        panel.add(color);
        color.getSelectionModel().addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent change){
                character.getDrawProperties().setBackgroundColor(color.getSelectionModel().getSelectedColor());
                animation.forceRedraw();
            }
        });
        character.getDrawProperties().addListener(new DrawPropertiesListener(){
            public void updateBackgroundColor(Color newColor){
                color.setColor(newColor);
                animation.forceRedraw();
            }
        });
        return panel;
    }

    private JComponent makeEvents(final Animation animation, final DrawArea area){
        final SwingEngine contextEditor = new SwingEngine("animator/animation-events.xml");
        final JScrollPane eventScroll = (JScrollPane) contextEditor.find("event-scroll");
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
            public Object invoke(Object a){
                currentEvent.set(a);
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
                    /*
                       JPanel editor = event.getEditor(animation);
                       JPanel work = (JPanel) contextEditor.find("event-work");
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
                       */

                    /*
                       if ( editor != null ){
                       JDialog dialog = new JDialog();
                       dialog.setSize(editor.getSize());
                       Point here = animEditor.getRootComponent().getLocation();
                       SwingUtilities.convertPointToScreen(here, animEditor.getRootComponent());
                       here.setLocation(here.getX() + animEditor.getRootComponent().getWidth() / 2, here.getY() + animEditor.getRootComponent().getHeight() / 2);
                       dialog.setLocation(here);
                       dialog.getContentPane().add(editor);
                       dialog.addWindowStateListener(new WindowStateListener(){
                       public void windowStateChanged(WindowEvent e){
                       / * should use a list update event here * /
                       eventList.setListData( animation.getEvents() );
                       }
                       });
                       dialog.show();
                       }
                       */
                }
            }
        });

        eventList.setCellRenderer(new DefaultListCellRenderer() {
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
