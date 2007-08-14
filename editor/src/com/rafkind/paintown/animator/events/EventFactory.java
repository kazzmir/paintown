package com.rafkind.paintown.animator.events;

import java.util.*;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.events.*;
import com.rafkind.paintown.Lambda0;

// Must populate when an event is added
		
public class EventFactory
{
	private static boolean inited = false;
	
	private static HashMap _events = new HashMap();
	
	public static void init()
	{
		if(!inited)
		{
			_events.put( new String("attack"), new Lambda0(){public Object invoke(){return new AttackEvent();}});
			_events.put( new String("bbox"), new Lambda0(){public Object invoke(){return new BBoxEvent();}});
			_events.put( new String("coords"), new Lambda0(){public Object invoke(){return new CoordsEvent();}});
			_events.put( new String("delay"), new Lambda0(){public Object invoke(){return new DelayEvent();}});
			_events.put( new String("face"), new Lambda0(){public Object invoke(){return new FaceEvent();}});
			_events.put( new String("frame"), new Lambda0(){public Object invoke(){return new FrameEvent();}});
			_events.put( new String("jump"), new Lambda0(){public Object invoke(){return new JumpEvent();}});
			_events.put( new String("move"), new Lambda0(){public Object invoke(){return new MoveEvent();}});
			_events.put( new String("nop"), new Lambda0(){public Object invoke(){return new NopEvent();}});
			_events.put( new String("offset"), new Lambda0(){public Object invoke(){return new OffsetEvent();}});
			_events.put( new String("projectile"), new Lambda0(){public Object invoke(){return new ProjectileEvent();}});
			_events.put( new String("shadow"), new Lambda0(){public Object invoke(){return new ShadowEvent();}});
			_events.put( new String("sound"), new Lambda0(){public Object invoke(){return new SoundEvent();}});
			_events.put( new String("status"), new Lambda0(){public Object invoke(){return new StatusEvent();}});
			_events.put( new String("z-distance"), new Lambda0(){public Object invoke(){return new ZDistanceEvent();}});
			
			inited = true;
		}
	}
	
	public static AnimationEvent getEvent(String name)
	{
		try
		{
			return (AnimationEvent)(((Lambda0) _events.get( name )).invoke_());
		}
		catch (Exception e)
		{
			
		}
		
		return null;
	}
	
	public static Vector getNames()
	{
		return new Vector(_events.keySet());
	}
	
	public EventFactory()
	{
		// Nothing
	}
}
