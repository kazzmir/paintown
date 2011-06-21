package com.rafkind.paintown.animator.events.scala

// import java.util._;
import java.awt
import java.awt.event._;
import javax.swing._;
import javax.swing.event._;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.BoundingBox;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

class AttackEvent extends AnimationEvent {
  val DEFAULT_FORCE_X = 1.7
  val DEFAULT_FORCE_Y = 4.4

  var onDestroy = () => { }
  var attacks:List[Attack] = List[Attack]()

  class Attack(){
    var x1:Int = 0
    var y1:Int = 0
    var x2:Int = 0
    var y2:Int = 0
    var damage:Int = 0
    var forceX:Double = DEFAULT_FORCE_X
    var forceY:Double = DEFAULT_FORCE_Y
  }

  override def destroy() = {
    onDestroy()
  }

  override def loadToken(token:Token){
    this.attacks = List[Attack]()
        
    val attack = parse(token);

        /*
        Attack attack = new Attack();

        Token x1_token = token.findToken("x1");
        if (x1_token != null){
            attack.x1 = x1_token.readInt(0);
        }

        Token y1_token = token.findToken("y1");
        if (y1_token != null){
            attack.y1 = y1_token.readInt(0);
        }

        Token x2_token = token.findToken("x2");
        if (x2_token != null){
            attack.x2 = x2_token.readInt(0);
        }

        Token y2_token = token.findToken("y2");
        if (y2_token != null){
            attack.y2 = y2_token.readInt(0);
        }

        Token damage_token = token.findToken("damage");
        if (damage_token != null){
            attack.damage = damage_token.readInt(0);
        }

        Token force_token = token.findToken("force");
        if (force_token != null){
            attack.force = force_token.readInt(0);
        }
        */

        if (attack.x1 != 0 || attack.y1 != 0 ||
            attack.x2 != 0 || attack.y2 != 0 ||
            attack.damage != 0){
            attacks = attack :: attacks;
        }

        /* FIXME
        for (box <- token.findTokens("box")){
            attacks = attacks ++ parse(box);
        }
        */
    }

  def parse(token:Token):Attack = {
        val attack = new Attack();

        val x1_token = token.findToken("x1");
        if (x1_token != null){
            attack.x1 = x1_token.readInt(0);
        }

        val y1_token = token.findToken("y1");
        if (y1_token != null){
            attack.y1 = y1_token.readInt(0);
        }

        val x2_token = token.findToken("x2");
        if (x2_token != null){
            attack.x2 = x2_token.readInt(0);
        }

        val y2_token = token.findToken("y2");
        if (y2_token != null){
            attack.y2 = y2_token.readInt(0);
        }

        val damage_token = token.findToken("damage");
        if (damage_token != null){
            attack.damage = damage_token.readInt(0);
        }

        val force_token = token.findToken("force");
        if (force_token != null){
            /*
            attack.force = force_token.readInt(0);
            */
            try{
                val x = force_token.readDouble(0);
                val y = force_token.readDouble(1);
                attack.forceX = x;
                attack.forceY = y;
            } catch {
              case fail:NoSuchElementException => {
              }
            }
        }

        return attack;
    }

  override def getToken():Token = {
        val temp = new Token();
        /*
        temp.addToken(new Token("attack"));
        for (Attack attack : attacks){
            if (attack.x1 == 0 && attack.y1 == 0 && attack.x2 == 0 && attack.y2 == 0 && attack.damage == 0) {
                // temp.addToken(new Token("box"));
            } else {
                Token box = new Token();
                temp.addToken(box);
                box.addToken(new Token("box"));
                box.addToken(new String[]{"x1",Integer.toString(attack.x1)});
                box.addToken(new String[]{"y1",Integer.toString(attack.y1)});
                box.addToken(new String[]{"x2",Integer.toString(attack.x2)});
                box.addToken(new String[]{"y2",Integer.toString(attack.y2)});
                box.addToken(new String[]{"force", Double.toString(attack.forceX), Double.toString(attack.forceY)});
                box.addToken(new String[]{"damage",Integer.toString(attack.damage)});
            }
        }
        */

        temp
    }


    override def getName():String = {
      getToken().toString()
    }

    override def getEditor(animation:Animation, area:DrawArea):JPanel = {
        /*
        if (attacks.isEmpty()){
            attacks.add(new Attack());
            return getEditor(animation, area, attacks.get(0));
        } else {
            return getEditor(animation, area, attacks.get(0));
        }
        */
       null
    }

    override def interact(animation:Animation){
      /*
        if (attacks.isEmpty()){
            animation.setAttack(new BoundingBox(0, 0, 0, 0));
        } else {
            Attack attack = attacks.get(0);
            animation.setAttack(new BoundingBox(attack.x1, attack.y1, attack.x2, attack.y2));
        }
        */
    }

/*
    private JPanel getEditor(final Animation animation, final DrawArea area, final Attack attack){
        SwingEngine engine = new SwingEngine("animator/eventattack.xml");
        // ((JPanel)engine.getRootComponent()).setSize(200,150);

        final JSpinner x1spin = (JSpinner) engine.find( "x1" );
        x1spin.setValue(new Integer(attack.x1));
        x1spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.x1 = ((Integer)x1spin.getValue()).intValue();
                interact(animation);
                animation.forceRedraw();
            }
        });
        final JSpinner y1spin = (JSpinner) engine.find( "y1" );
        y1spin.setValue(new Integer(attack.y1));
        y1spin.addChangeListener( new ChangeListener() {
            public void stateChanged(ChangeEvent changeEvent){
                attack.y1 = ((Integer)y1spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner x2spin = (JSpinner) engine.find( "x2" );
        x2spin.setValue(new Integer(attack.x2));
        x2spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.x2 = ((Integer)x2spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner y2spin = (JSpinner) engine.find( "y2" );
        y2spin.setValue(new Integer(attack.y2));
        y2spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.y2 = ((Integer)y2spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner forcespinX = (JSpinner) engine.find("forceX");
        forcespinX.setModel(new SpinnerNumberModel(attack.forceX, 0, 1000, 0.1));
        forcespinX.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.forceX = ((Double)forcespinX.getValue()).doubleValue();
            }
        });
        final JSpinner forcespinY = (JSpinner) engine.find("forceY");
        forcespinY.setModel(new SpinnerNumberModel(attack.forceY, 0, 1000, 0.1));
        forcespinY.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.forceY = ((Double)forcespinY.getValue()).doubleValue();
            }
        });

        final JSpinner damagespin = (JSpinner) engine.find( "damage" );
        damagespin.setValue(new Integer(attack.damage));
        damagespin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                attack.damage = ((Integer)damagespin.getValue()).intValue();
            }
        });

        final JButton toggle = (JButton) engine.find("toggle");
        toggle.addActionListener(new AbstractAction(){
            boolean toggled = false;
            MouseInputAdapter listener = new MouseInputAdapter(){
                public void mousePressed(MouseEvent e){
                    attack.x1 = (int)(e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX());
                    attack.y1 = (int)(e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY());
                    x1spin.setValue(new Integer(attack.x1));
                    y1spin.setValue(new Integer(attack.y1));
                    interact(animation);
                    animation.forceRedraw();
                }

                public void mouseDragged(MouseEvent e){
                    attack.x2 = (int)(e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX());
                    attack.y2 = (int)(e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY());
                    x2spin.setValue(new Integer(attack.x2));
                    y2spin.setValue(new Integer(attack.y2));
                    interact(animation);
                    animation.forceRedraw();
                }
            };

            public void actionPerformed(ActionEvent event){
                if (toggled){
                    toggle.setText("Draw attack box");
                    area.enableMovement();
                    area.removeMouseListener(listener);
                    area.removeMouseMotionListener(listener);
                    onDestroy = new Lambda0(){
                        public Object invoke(){
                            return null;
                        }
                    };
                } else {
                    toggle.setText("Stop drawing");
                    area.disableMovement();
                    area.addMouseListener(listener);
                    area.addMouseMotionListener(listener);
                    onDestroy = new Lambda0(){
                        public Object invoke(){
                            area.removeMouseListener(listener);
                            area.removeMouseMotionListener(listener);
                            area.enableMovement();
                            return null;
                        }
                    };
                }

                toggled = ! toggled;
            }
        });

        return (JPanel)engine.getRootComponent();

    }
    */
}
