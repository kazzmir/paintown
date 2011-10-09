package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import javax.swing.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;

public interface AnimationEvent{
    public void interact(Animation animation);
    public String getName();
    public JPanel getEditor(Animation animation, DrawArea area);
    public Token getToken();
    public void loadToken(Token token);
    public void destroy();
    public AnimationEvent copy();
}
