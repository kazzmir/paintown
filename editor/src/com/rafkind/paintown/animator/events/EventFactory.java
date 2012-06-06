package com.rafkind.paintown.animator.events;

import java.util.*;
import com.rafkind.paintown.Lambda0;

// Must populate when an event is added
		
public class EventFactory{
    private static HashMap events = new HashMap();
    private static List ignoreEvents = new ArrayList();

    private EventFactory(){
        // Nothing
    }

    public static void init(){
        events.put( "attack", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.AttackEvent();}});
        events.put( "bbox", new Lambda0(){public Object invoke(){return new BBoxEvent();}});
        events.put( "coords", new Lambda0(){public Object invoke(){return new CoordsEvent();}});
        events.put( "delay", new Lambda0(){public Object invoke(){return new DelayEvent();}});
        events.put( "defense", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.DefenseEvent();}});
        events.put( "effect", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.EffectEvent();}});
        events.put( "face", new Lambda0(){public Object invoke(){return new FaceEvent();}});
        events.put( "frame", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.FrameEvent();}});
        events.put( "hittable", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.HittableEvent();}});
        events.put( "jump", new Lambda0(){public Object invoke(){return new JumpEvent();}});
        events.put( "move", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.MoveEvent();}});
        events.put( "nop", new Lambda0(){public Object invoke(){return new NopEvent();}});
        events.put( "next-ticket", new Lambda0(){public Object invoke(){return new TicketEvent();}});
        events.put( "offset", new Lambda0(){public Object invoke(){return new OffsetEvent();}});
        events.put( "relative-offset", new Lambda0(){public Object invoke(){return new com.rafkind.paintown.animator.events.scala.RelativeOffsetEvent();}});
        events.put( "projectile", new Lambda0(){public Object invoke(){return new ProjectileEvent();}});
        events.put("trail", new Lambda0(){public Object invoke(){ return new TrailEvent();}});
        events.put( "shadow", new Lambda0(){public Object invoke(){return new ShadowEvent();}});
        events.put( "sound", new Lambda0(){public Object invoke(){return new SoundEvent();}});
        events.put( "user", new Lambda0(){public Object invoke(){return new UserDefinedEvent();}});
        events.put( "status", new Lambda0(){public Object invoke(){return new StatusEvent();}});
        events.put( "z-distance", new Lambda0(){public Object invoke(){return new ZDistanceEvent();}});

        ignoreEvents.add("basedir");
        ignoreEvents.add("range");
        ignoreEvents.add("keys");
        ignoreEvents.add("type");
        ignoreEvents.add("name");
        ignoreEvents.add("loop");
        ignoreEvents.add("sequence");
    }

    static{
        init();
    }

    public static AnimationEvent getEvent(String name){
        if ( ignoreEvents.contains( name ) ){
            return null;
        }
        try{
            return (AnimationEvent)(((Lambda0) events.get( name )).invoke());
        } catch (Exception e) {
            System.out.println( "Could not get event '" + name + "'" );
            return null;
            // e.printStackTrace();
        }
    }

    public static Vector getNames(){
        Object[] names = events.keySet().toArray();
        Arrays.sort(names);
        return new Vector(Arrays.asList(names));
    }
}
