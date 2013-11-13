#ifndef COMMONROOTBASE_H
#define COMMONROOTBASE_H

//just to silent ROOT warning about dict being loaded twice
class CommonRootBase {

  public:
     CommonRootBase();
     ~CommonRootBase();  

  protected:
     static bool m_loaded_builtin_dicts;

};

#endif
