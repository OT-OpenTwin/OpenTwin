import OpenTwin

def Test(this):
    topRow = OpenTwin.GetPropertyValue(this,"Top row")
    bottomRow = OpenTwin.GetPropertyValue(this,"Bottom row")
    
    setSuccess = OpenTwin.SetPropertyValue(this,"Top row",topRow+1)
    if not setSuccess:    
        raise Exception('Failed to update property of ', this)
    
    setSuccess = OpenTwin.SetPropertyValue(this,"Bottom row", bottomRow+1)
    if not setSuccess:    
        raise Exception('Failed to update property of ', this)

def __main__(this):
    Test(this)