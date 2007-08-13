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
			_events.put( new String("move"), new Lambda0(){public Object invoke(){return new MoveEvent();}});
			_events.put( new String("attack"), new Lambda0(){public Object invoke(){return new AttackEvent();}});
			_events.put( new String("bbox"), new Lambda0(){public Object invoke(){return new BBoxEvent();}});
			_events.put( new String("coords"), new Lambda0(){public Object invoke(){return new CoordsEvent();}});
			_events.put( new String("delay"), new Lambda0(){public Object invoke(){return new DelayEvent();}});
			_events.put( new String("face"), new Lambda0(){public Object invoke(){return new FaceEvent();}});
			_events.put( new String("frame"), new Lambda0(){public Object invoke(){return new FrameEvent();}});
			
			inited = true;
		}
	}
	
	public static AnimationEvent getEvent(String name)
	{
		return (AnimationEvent)(((Lambda0) _events.get( name )).invoke_());
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
