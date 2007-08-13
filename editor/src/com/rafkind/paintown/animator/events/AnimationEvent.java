package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;

public interface AnimationEvent
{
	public void interact(DrawArea area);
	
	public String getName();
	
	public JDialog getEditor();
	
	public Token getToken();
	
	public void loadToken(Token token);
}
