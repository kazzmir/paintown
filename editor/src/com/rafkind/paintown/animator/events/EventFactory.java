package com.rafkind.paintown.animator.events;

import java.util.*;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.events.MoveEvent;

// Must populate when an event is added
		
public class EventFactory
{
	public static AnimationEvent getEvent(String name)
	{
		if(name.equals("move"))return new MoveEvent();
		
		
		return null;
	}
	
	public static Vector getNames()
	{
		Vector names = new Vector();
		
		names.addElement(new String("move"));
		
		return names;
	}
	
	public EventFactory()
	{
		// Nothing
	}
}
