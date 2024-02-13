package com.rafkind.paintown.animator;

import java.util.*;

public class IQueue
{
	private Vector q = new Vector();
	
	public IQueue()
	{
		// Nothing
	}
	
	public Object pop()
	{
		Object item = q.get(q.size());
		q.remove(q.size());
		return item;
	}
	
	public void push(Object item)
	{
		q.addElement(item);
	}
	
	public void clear()
	{
		q.clear();
	}
}
