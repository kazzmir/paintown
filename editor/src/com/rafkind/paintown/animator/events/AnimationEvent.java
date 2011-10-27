package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;

public interface AnimationEvent{
    /* update an animation with the event semantics */
    public void interact(Animation animation);
    
    /* name of the event */
    public String getName();

    /* a dialog widget that has buttons for manipulating this event */
    public JPanel getEditor(Animation animation, DrawArea area);

    /* serialize this event to an s-expression */
    public Token getToken();

    /* set up the properties of this event given the serialized form */
    public void loadToken(Token token);

    /* destroy any data that is kept in the animation */
    public void destroy();

    /* get a copy of this object */
    public AnimationEvent copy();

    /* get a description of what this event does */
    public String getDescription();
}
