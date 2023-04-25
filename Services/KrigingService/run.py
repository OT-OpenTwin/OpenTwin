print('\n\nExternal Python program running...\n')
import os

path = os.getcwd()

print(path,"\n")

import numpy as np
import matplotlib.pyplot as plt

# Returns the sum of two numbers.
def add(a, b):
    return a + b + 1


def test():
    #import numpy as np
    #import matplotlib.pyplot as plt

    from smt.problems import Rosenbrock

    ndim = 3
    problem = Rosenbrock(ndim=ndim)

    num = 1000
    x = np.ones((num, ndim))
    x[:, 0] = np.linspace(-2, 2, num)
    x[:, 1] = np.linspace(14, 20, num)
    x[:, 2] = np.linspace(85, 99, num)
    y = problem(x)

    yd = np.empty((num, ndim))
    for i in range(ndim):
        yd[:, i] = problem(x, kx=i).flatten()


    
    print(x[:5])
    print(x.shape, y.shape)
    print(yd.shape)

    plt.plot(x[:, 0], y[:, 0])
    plt.xlabel("x")
    plt.ylabel("y")
    plt.show()



def testParam(theta, nugget, corr, poly):
    #import numpy as np;
    print("hello there")
    print("params received", theta, nugget, corr, poly)

    return 0;

# default theta0 = 0.01
# Correlation function type corr = [�abs_exp�, �squar_exp�, �act_exp�, �matern52�, �matern32�] default = squar_exp
# Regression function type poly = [�constant�, �linear�, �quadratic�] default = constant
# default nugget = 2.220446049250313e-14
def kriging(theta0, corr, poly, nugget, train, test, inputcol, outputcol, display_plot = False, display_output = False):
    #from mylib import executeKriging as krg

    # prepare train data
    xtrain = train[:, inputcol]
    ytrain = train[:, outputcol]

    # prepare test data
    xtest = test[:, inputcol]
    ytest = test[:, outputcol]

    # initialize model
    from smt.surrogate_models import KRG
    sm = KRG(theta0=[theta0], print_global=False, corr = corr, poly= poly, nugget= nugget)

    # train model
    sm.set_training_values(xtrain, ytrain[:,0])
    sm.train()

    ##### Training verification.
    trpredict = sm.predict_values(xtrain)
    import math
    e = ytrain - trpredict
    n = ytrain.shape[0]
    trError = math.sqrt(np.sum(e**2)/n)

    ##### Testing
    tepredict = sm.predict_values(xtest)
    ##### Testing rmse Error
    import math
    err = ytest - tepredict
    n = ytest.shape[0]
    teError = math.sqrt(np.sum(err**2)/n)

    ##### printing
    if(display_output == True):
        troutput = np.concatenate((ytrain, trpredict), axis=1)
        teoutput = np.concatenate((ytest, tepredict), axis=1)
        print('train: ', xtrain[:5], '\n', 'ytrain: ', trrows[:5] , '\n', 
              'test: ', xtest[:5], '\n', 'ytest: ', teoutput[:5], '\n')
        print(xtrain.shape,xtest.shape, theta0,corr, poly, nugget, "\t", trError,"\t\t", teError)
        print('dimensions',len(inputcol))

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    tracc = calculateAccuracy(ytrain, trpredict, 1e-2)
    teacc = calculateAccuracy(ytest, tepredict, 1e-2)


    if(display_plot == True):
        ##### plotting graph
        if len(inputcol) == 1:
#             print('printing 2D graph plot')
            import matplotlib.pyplot as plt
            fig = plt.figure(figsize=(20,10))

            ax = fig.add_subplot(121)
        #     plt.subplot(1, 2, 1)
            ax.title.set_text('train: ' + str(trError))
            ax.scatter(xtrain, ytrain, color = 'blue',label='train')
            ax.scatter(xtrain, trpredict, color = 'green',label='predict')
            ax.legend(fontsize=16, frameon=False)

            ax = fig.add_subplot(122)
        #     plt.subplot(1, 2, 2)
            ax.title.set_text('test: ' + str(teError))
            ax.scatter(xtest, ytest, color = 'black',label='test')
            ax.scatter(xtest, tepredict, color = 'red',label='predict')
            ax.legend(fontsize=16, frameon=False)
            
#             plt.show()

        if len(inputcol) == 2:
#             print('printing 3D graph plot')
            import matplotlib.pyplot as plt
            # Plot X,Y,Z
            fig = plt.figure(figsize=(20,10))

            ax = fig.add_subplot(121, projection='3d')
            ax.title.set_text('train: ' + str(trError))
            ax.scatter(xtrain[:,0], xtrain[:,1], ytrain[:,0], c='red', label='train')
        #     ax.scatter(xtrain[:,0], xtrain[:,1], savedcopy, c='blue', label='withouterror')
            ax.scatter(xtrain[:,0], xtrain[:,1], trpredict[:,0], c='green',label='predict')
            ax.legend(fontsize=16, frameon=False)

            ax = fig.add_subplot(122, projection='3d')
            ax.title.set_text('test: ' + str(teError))
            ax.scatter(xtest[:,0],xtest[:,1],ytest[:,0], color='yellow',label='test')
            ax.scatter(xtest[:,0],xtest[:,1],tepredict[:,0], color='magenta',label='predict')
            ax.legend(fontsize=16, frameon=False)
        
#             plt.show()
        
    return sm, trpredict, tepredict, trError, teError, tracc, teacc


# accuracy = num_of_correct_prediction / num_of_prediction
def calculateAccuracy(y, predict, tolerance, display_output = False):
    import numpy as np
    
    count = 0
    for i in range(y.shape[0]):
#         print(i,y[i], predict[i], y[i] - predict[i], abs(y[i] - predict[i]), abs(y[i] - predict[i]) <= tolerance)
#         -1 - 1 = 2 <= 0.1     :   F
#         -1 - -1 = 0 <= 0.1    :   T
#         1 - 1 = 0 <= 0.1      :   T
#         1- -1 = 2 <= 0.1      :   F
        if(abs(y[i] - predict[i]) <= tolerance):
            count = count + 1
        
    acc = count/predict.shape[0]
    
    if(display_output == True):
        print('\n\ntolerance: ', tolerance)
        print('accuracy('+str(count)+'/'+str(predict.shape[0])+') : ', acc)
    
    return acc



################### ADDING NOISE #######################################
def addNoise(train, test, ndim, sd = 0.01, display_output = False):
    #################### noise in training set ###############################
    noise = np.random.normal(0,sd,train.shape[0])
    if(display_output == True):
        print('max noise value: ', max(noise))
        print('output col: ', ndim)
    
    temp = train[:,ndim]
#     print('NOISE CHECK: ', noise.shape,  noise[:5])
    savedcopy = temp *1
    op = temp+noise
    if(display_output == True):
        print('output shape: ', op.shape,savedcopy.shape)
    train[:, ndim]= op
    if(display_output == True):
        print('\n\n\n final: ',train.shape, '\nnoise: \t', noise[:5], '\nsaved: \t', savedcopy[:5], '\nnoisy: \t', temp[:5], '\ntrain: \t', train[:5])
    
    #################### noise in testing set ###############################
    noise = np.random.normal(0,sd,test.shape[0])
    if(display_output == True):
        print('max noise value: ', max(noise))
    temp = test[:,ndim]
    op = temp+noise
    test[:, ndim]= op
    
    return train, test


def generate3DRosenbrock(nrows):
    import numpy as np

    from smt.problems import Rosenbrock

    ndim = 3
    problem = Rosenbrock(ndim=ndim)

    num = nrows
    x = np.ones((num, ndim))

    x[:, 0] = np.linspace(-2, 2, num)
    x[:, 1] = np.linspace(1, 2, num)
    x[:, 2] = np.linspace(8, 12, num)

    y = problem(x)
    data = np.concatenate((x , y), axis=1)
    return data


def executeKriging(a,b,c,d):
    print('params', a, b, c , d)
    
    ds = generate3DRosenbrock(1000)
    ndim = 3
    inputcol, outputcol =np.array(range(ndim)), [ndim]
    print('INPUT COLS: ', inputcol, 'OUTPUT COLS: ', outputcol)

    ndim=len(inputcol)

    # default theta0 = 0.01
    theta0 = 0.01
    # Correlation function type = [�abs_exp�, �squar_exp�, �act_exp�, �matern52�, �matern32�] default = squar_exp
    corr_func_type =  ['abs_exp', 'squar_exp', 'matern52', 'matern32']
    corr = 'squar_exp'
    # Regression function type = [�constant�, �linear�, �quadratic�] default = constant
    reg_func_type = ['constant', 'linear', 'quadratic']
    poly = 'linear'
    # default nugget = 2.220446049250313e-14
    nugget= 2.220446049250313e-14

    # trrows = 200
    add_noise = False
    sd = 1
    print('noise: ', str(add_noise),'\t', str(sd), 
          '\ntheta: ', str(theta0), 
          '\ncorr: ', corr, 
          '\npoly: ', poly, 
          '\nnugget: ', str(nugget),
          '\ndim: ', ndim
         )
    display_plot = False

    #######################################################################
    #################### SEPARATE TRAIN TEST DATA #########################
    #######################################################################
    from sklearn.model_selection import train_test_split
    # tr, te = train_test_split(ds, test_size=100)
    total_rows = ds.shape[0]

    outputs = []

    start = 50
    step = 100
    upperlimit = 1000 #2500 # total_rows - (step)
    print('start: ', start, '\tstep: ', step ,'\tstop: ', upperlimit)
    if int(upperlimit / step) >10:
        display_plot = False

    for i in range(start, upperlimit, step):
        trrows = i
        terows = total_rows - trrows
    #     print(trrows, terows)

        train, test = train_test_split(ds, test_size=terows)
        if(add_noise == True):
            train, test = addNoise(train, test, ndim, sd, False)
    
    #     for r in reg_func_type:
    #         poly = r

    #         for c in corr_func_type:
    #             corr = c
        try:            
            sm, trpredict, tepredict, trError, teError, tracc, teacc = kriging(theta0, corr, poly, nugget, train, test, 
                                                                       inputcol, outputcol, display_plot, False)

            output = [train.shape[0], test.shape[0], theta0, corr, poly, trError, teError, tracc, teacc, sm]
            outputs.append(output)
        except Exception as e:
    #         print('exception occured')
    #         e.print
            output = [train.shape[0], test.shape[0], theta0, 'ERROR', str(e), 0, 0, 0, 0, None]
            outputs.append(output)

    print('\n\n######################### OUTPUTS: #################################')
    for o in outputs:
        print(o[0],'\t',o[1],'\t',o[2],'\t',o[3],'\t',o[4],'\t',"{:.15e}".format(o[5]),'\t',"{:.15e}".format(o[6]),'\t',"{:.4f}".format(o[7]),'\t',"{:.4f}".format(o[8]))

    
    ## plotting
    output_list = np.array(outputs, dtype=object)
    # print(output_list)
    x1 = output_list[:, 0]

    import matplotlib.pyplot as plt
    fig = plt.figure(figsize=(20,10))
    plt.title('outputs:\n'+ corr + '\n' + poly)
    plt.plot(x1, output_list[:,5], color = 'blue',label='train')
    plt.plot(x1, output_list[:,6], color = 'green',label='predict')
    plt.legend(fontsize=16, frameon=False)
    plt.show()



#def train(train, theta0, corr, poly , nugget):
def train(theta0, nugget, corr, poly):
    print('executing training with params', theta0, nugget, corr, poly)

    import numpy as np
    import math


    ds = generate3DRosenbrock(1000)
    ndim = 3
    inputcol, outputcol =np.array(range(ndim)), [ndim]
    print('INPUT COLS: ', inputcol, 'OUTPUT COLS: ', outputcol)

    # separate train test data
    from sklearn.model_selection import train_test_split
    total_rows = ds.shape[0]
    outputs = []
    trrows = 300
    terows = total_rows - trrows
    train, test = train_test_split(ds, test_size=terows)


    print("training data: \t", train.shape, "\n", train[:5])
    print("training data: \t", test.shape, "\n", test[:5])

     # prepare train data
    global xtrain 
    xtrain = train[:, inputcol]
    global ytrain 
    ytrain = train[:, outputcol]

    # initialize model
    from smt.surrogate_models import KRG
    global sm 
    sm = KRG(theta0=[theta0], print_global=False, corr = corr, poly= poly, nugget= nugget)
    print('\nmodel', sm, '\n\n\n')

    # train model
    sm.set_training_values(xtrain, ytrain[:,0])
    sm.train()

    ##### Training verification.
    trpredict = sm.predict_values(xtrain)
    e = ytrain - trpredict
    n = ytrain.shape[0]
    trError = math.sqrt(np.sum(e**2)/n)

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    tracc = calculateAccuracy(ytrain, trpredict, 1e-2)
    
    # prepare test data
    global xtest 
    xtest = test[:, inputcol]
    global ytest 
    ytest = test[:, outputcol]

    ##### Testing
    tepredict = sm.predict_values(xtest)
    err = ytest - tepredict
    n = ytest.shape[0]
    teError = math.sqrt(np.sum(err**2)/n)

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    teacc = calculateAccuracy(ytest, tepredict, 1e-2)


    print('trError\t\t\t', 'teError\t\t', 'tracc\t','teacc')
    print(trError, '\t', teError, tracc, '\t', teacc)

    import jsonpickle
    serialized_sm = jsonpickle.encode(sm)
    print(type(serialized_sm), len(serialized_sm), '\n\n', serialized_sm[:1000])

    #return serialized_sm
    return (trError, teError, tracc, teacc, serialized_sm)


def train(theta0, nugget, corr, poly, data):
    print('executing training with params', theta0, nugget, corr, poly)

    import numpy as np
    import math


    ds = generate3DRosenbrock(1000)
    ndim = 3
    inputcol, outputcol =np.array(range(ndim)), [ndim]
    print('INPUT COLS: ', inputcol, 'OUTPUT COLS: ', outputcol)

    # separate train test data
    from sklearn.model_selection import train_test_split
    total_rows = ds.shape[0]
    outputs = []
    trrows = 300
    terows = total_rows - trrows
    train, test = train_test_split(ds, test_size=terows)


    print("training data: \t", train.shape, "\n", train[:5])
    print("test data: \t", test.shape, "\n", test[:5])

     # prepare train data
    global xtrain 
    xtrain = train[:, inputcol]
    global ytrain 
    ytrain = train[:, outputcol]

    # initialize model
    from smt.surrogate_models import KRG
    global sm 
    sm = KRG(theta0=[theta0], print_global=False, corr = corr, poly= poly, nugget= nugget)
    print('\nmodel', sm, '\n\n\n')

    # train model
    sm.set_training_values(xtrain, ytrain[:,0])
    sm.train()

    ##### Training verification.
    trpredict = sm.predict_values(xtrain)
    e = ytrain - trpredict
    n = ytrain.shape[0]
    trError = math.sqrt(np.sum(e**2)/n)

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    tracc = calculateAccuracy(ytrain, trpredict, 1e-2)
    
    # prepare test data
    global xtest 
    xtest = test[:, inputcol]
    global ytest 
    ytest = test[:, outputcol]

    ##### Testing
    tepredict = sm.predict_values(xtest)
    err = ytest - tepredict
    n = ytest.shape[0]
    teError = math.sqrt(np.sum(err**2)/n)

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    teacc = calculateAccuracy(ytest, tepredict, 1e-2)


    print('trError\t\t\t', 'teError\t\t', 'tracc\t','teacc')
    print(trError, '\t', teError, tracc, '\t', teacc)

    import jsonpickle
    serialized_sm = jsonpickle.encode(sm)
    print(type(serialized_sm), len(serialized_sm), '\n\n', serialized_sm[:1000])

    #return serialized_sm
    return (trError, teError, tracc, teacc, serialized_sm)



def array_test(a):
    print("a.shape={}, a.dtype={}".format(a.shape, a.dtype))
    print("received array: \n", a)
    #a *= 2

    #ds = generate3DRosenbrock(10)
    ds = a[:,:3]
    return a
    #return ds

def get_model():
    #if 'myVar' in globals():
    ## myVar exists.
    import jsonpickle
    serialized_sm = jsonpickle.encode(sm)
    print(type(serialized_sm), len(serialized_sm), '\n\n', serialized_sm[:1000])
    return serialized_sm

def get_training_set():
    return xtest

def predict(modelJson, testds):
    print('model received 112',type(modelJson),  len(modelJson), '\n',  modelJson[:200])
    print('\ndataset received for prediction\n', testds[:3])
    import jsonpickle
    model = jsonpickle.decode(modelJson)

    import numpy as np
    import math

    ##### Testing
    tepredict = model.predict_values(xtest)
    err = ytest - tepredict
    n = ytest.shape[0]
    teError = math.sqrt(np.sum(err**2)/n)

    ##### accuracy
    # accuracy = num_of_correct_prediction / num_of_prediction
    teacc = calculateAccuracy(ytest, tepredict, 1e-2)

    print('\ntest results:\n', tepredict.shape, teError, teacc)
    print('outputs: ', testds[:, 0].shape, "\n")
    retVal = tepredict[:testds.shape[0]]
    print(retVal)
    return retVal


def test(modelJson, testds):
    print('model received',type(modelJson),  len(modelJson), '\n',  modelJson[:200])
    print('\ndataset received\n', testds[:3])

    print('\n return data: \n', testds[:, 0])

    return testds[:, 0]
