package com.rafkind.paintown;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.SpinnerModel;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class MinMaxSpinnerModel implements SpinnerModel {
	List listeners = new ArrayList();
	int value;
	int min;
	int max;

	public MinMaxSpinnerModel( int value, int min, int max ){
		this.value = value;
		this.min = min;
		this.max = max;
	}

	public Object getNextValue(){
		return max > value ? new Integer( value + 1 ) : null;
	}

	public void addChangeListener( ChangeListener l ){
		listeners.add( l );
	}

	public void removeChangeListener( ChangeListener l ){
		listeners.remove( l );
	}

	public Object getPreviousValue(){
		return value > min ? new Integer( value - 1 ) : null;
	}

	public void setValue( Object o ){
		value = ((Integer) o).intValue();
		ChangeEvent event = new ChangeEvent( this );
		for ( Iterator it = listeners.iterator(); it.hasNext(); ){
			ChangeListener change = (ChangeListener) it.next();
			change.stateChanged( event );
		}
	}

	public Object getValue(){
		return new Integer( value );
	}
}
