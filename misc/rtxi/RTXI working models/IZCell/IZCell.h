/*
 * 
 */

#include <math.h>
#include <default_gui_model.h>

class IZCell : public DefaultGUIModel
{
    public:
    
        IZCell(void);
        virtual ~IZCell(void);
        
        void execute(void);
        
    protected:
    
        void update(DefaultGUIModel::update_flags_t);
    
    private:
    
        double I_offset;
        double a, b, c, d;
        double V, u;
        double steps, period;
};
