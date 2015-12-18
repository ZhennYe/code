function ArtStimRemPopup(handles, funVal)

set(handles.parm1text,'Visible','off');
set(handles.parm1val,'Visible','off');
set(handles.parm2text,'Visible','off');
set(handles.parm2val,'Visible','off');
set(handles.parm3text,'Visible','off');
set(handles.parm3val,'Visible','off');
set(handles.parm4text,'Visible','off');
set(handles.parm4val,'Visible','off');
set(handles.parm5text,'Visible','off');
set(handles.parm5val,'Visible','off');
set(handles.parm6text,'Visible','off');
set(handles.parm6val,'Visible','off');
set(handles.pushbutton10,'Visible','off');



switch (funVal)
    case 1
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
    case 2
        set(handles.parm1text,'Visible','on','String','Segments/Mean');
        set(handles.parm1val,'Visible','on','String','30');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
    case 3
        set(handles.parm1text,'Visible','on','String','Burst Length');        
        set(handles.parm1val,'Visible','on','String','40');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
    case 4
        set(handles.parm1text,'Visible','on','String','Burst/Mean');
        set(handles.parm1val,'Visible','on','String','30');
        set(handles.parm2text,'Visible','on','String','Burst Length');        
        set(handles.parm2val,'Visible','on','String','40');
        set(handles.parm3text,'Visible','on','String','Leading zeros');        
        set(handles.parm3val,'Visible','on','String','2');
        set(handles.parm4text,'Visible','on','String','Trailing zeros');        
        set(handles.parm4val,'Visible','on','String','1');
    case 5
        set(handles.parm1text,'Visible','on','String','Segments/Mean');
        set(handles.parm1val,'Visible','on','String','30');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
    case 6
        set(handles.parm1text,'Visible','on','String','Polynom degree');
        set(handles.parm1val,'Visible','on','String','8');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
        set(handles.pushbutton10,'Visible','on');
       
    case 7
        set(handles.parm1text,'Visible','on','String','Polynom degree');
        set(handles.parm1val,'Visible','on','String','8');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
     case 8
        set(handles.parm1text,'Visible','on','String','Polynom degree');
        set(handles.parm1val,'Visible','on','String','8');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
        set(handles.parm4text,'Visible','on','String','Max interval (ms)');        
        set(handles.parm4val,'Visible','on','String','1000');
    case 9
        set(handles.parm1text,'Visible','on','String','Polynom degree');
        set(handles.parm1val,'Visible','on','String','8');
        set(handles.parm2text,'Visible','on','String','Leading zeros');        
        set(handles.parm2val,'Visible','on','String','4');
        set(handles.parm3text,'Visible','on','String','Trailing zeros');        
        set(handles.parm3val,'Visible','on','String','1');
        set(handles.parm4text,'Visible','on','String','Max fit (ms)');        
        set(handles.parm4val,'Visible','on','String','10');
end
        