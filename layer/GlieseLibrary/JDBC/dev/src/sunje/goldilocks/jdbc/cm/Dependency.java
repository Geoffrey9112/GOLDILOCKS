package sunje.goldilocks.jdbc.cm;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class Dependency
{
    protected boolean mIsFirst;
    protected boolean mIsLast;
    protected Dependency mOrigin;
    private Dependency mChild;
    
    Dependency()
    {
        mOrigin = this;
    }
    
    public Dependency alone()
    {
        mIsFirst = true;
        mIsLast = true;
        return this;
    }
    
    public Dependency first()
    {
        mIsFirst = true;
        mIsLast = false;
        return this;
    }
    
    public Dependency last()
    {
        mIsLast = true;
        return this;
    }

    public final Dependency derive()
    {
        if (mChild == null)
        {
            mChild = new DerivedDependency(this);
        }        
        mChild.mIsLast = false;
        return mChild;
    }
    
    Dependency next()
    {
        mIsFirst = false;
        return this;
    }
        
    byte getCode()
    {
        return mIsFirst ? (mIsLast ? Protocol.DEPENDENCY_ALONE : Protocol.DEPENDENCY_HEAD) :
                          (mIsLast ? Protocol.DEPENDENCY_TAIL  : Protocol.DEPENDENCY_BODY);
    }
}

class DerivedDependency extends Dependency
{
    DerivedDependency(Dependency aParent)
    {
        mOrigin = aParent.mOrigin;
    }
    
    public Dependency alone()
    {
        ErrorMgr.raiseRuntimeError("This method cannot be called");
        return null;
    }
    
    public Dependency first()
    {
        ErrorMgr.raiseRuntimeError("This method cannot be called");
        return null;
    }
    
    public Dependency last()
    {
        mIsLast = mOrigin.mIsLast;
        return this;
    }

    Dependency next()
    {
        mOrigin.mIsFirst = false;
        return this;
    }
    
    byte getCode()
    {
        return mOrigin.mIsFirst ? (mIsLast ? Protocol.DEPENDENCY_ALONE : Protocol.DEPENDENCY_HEAD) :
                                  (mIsLast ? Protocol.DEPENDENCY_TAIL  : Protocol.DEPENDENCY_BODY);
    }
}
