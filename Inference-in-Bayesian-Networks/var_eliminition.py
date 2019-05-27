import numpy as np
import copy

class Factor():
    def __init__(self, variables, CPT):
        self.variables = variables
        self.CPT = CPT

def restrict(factor, variable, value):
    print ("Restrict {0} = {1}".format(variable,value))
    if variable not in factor.variables:
        return factor
    else:
        index = factor.variables.index(variable)
        newCPT = {}
        for key in factor.CPT:
            if key[index] == value:
                newKey = key[:index] + key[index+1:]
                newCPT[newKey] = factor.CPT[key]
        newFactorVariables = copy.deepcopy(factor.variables)
        newFactorVariables.remove(variable)
        return Factor(newFactorVariables, newCPT)

def multiply(factor1, factor2):
    # print('multiplying')
    # printFactor(factor1)
    # printFactor(factor2)
    newCPT = {}
    commonVariables = list(set(factor1.variables) & set(factor2.variables))
    indexes1 = []
    indexes2 = []
    for i in range(len(commonVariables)):
        indexes1.append(factor1.variables.index(commonVariables[i]))
        indexes2.append(factor2.variables.index(commonVariables[i]))
    for key1, value1 in factor1.CPT.items():
        for key2, value2 in factor2.CPT.items():
            count = 0
            for i in range(len(commonVariables)):
                if key1[indexes1[i]] == key2[indexes2[i]]:
                    count += 1
            if count == len(commonVariables):
                start = 0
                newKey1 = ''
                newKey2 = key2
                orderedIndexes1 = indexes1
                orderedIndexes1.sort()
                for i in range(len(orderedIndexes1)):
                    newKey1 = newKey1 + key1[start:orderedIndexes1[i]]
                    start = orderedIndexes1[i]+1
                newKey1 = newKey1 + key1[start:]
                newKey = newKey1 + newKey2
                newCPT[newKey] = value1 * value2
    newFactorVariables1 = copy.deepcopy(factor1.variables)
    newFactorVariables2 = copy.deepcopy(factor2.variables)
    for i in range(len(commonVariables)):
        newFactorVariables1.remove(commonVariables[i])
    newFactorVariables = newFactorVariables1 + newFactorVariables2
    printFactor(Factor(newFactorVariables, newCPT))
    return Factor(newFactorVariables, newCPT)

def sumout(factor, variable):
    # print("Sum out {0} in factor:".format(variable))
    # printFactor(factor)
    if variable not in factor.variables:
        return factor
    else:
        index = factor.variables.index(variable)
        newCPT = {}
        for key, value in factor.CPT.items():
            newKey = key[:index] + key[index+1:]
            if newKey in newCPT:
                newCPT[newKey] += value
            else:
                newCPT[newKey] = value
        newFactorVariables = copy.deepcopy(factor.variables)
        newFactorVariables.remove(variable)
        return Factor(newFactorVariables, newCPT)

def normalize(factor):
    summation = sum(factor.CPT.values())
    newCPT = {k: v/summation for k,v in factor.CPT.items()}
    return Factor(factor.variables, newCPT)


def inference(factorList, queryVariables, orderedListOfHiddenVariables, evidenceList):

    factors = []

    if evidenceList:
        for factor in factorList:
            for evidence in evidenceList:
                if evidence in factor.variables:
                    factor = restrict(factor, evidence, evidenceList[evidence])
                    printFactor(factor)

            factors.append(factor)
    else:
        factors = copy.deepcopy(factorList)

    for var in orderedListOfHiddenVariables:

        newFactorList = []
        for factor in factors:
            if var in factor.variables:
                newFactorList.append(factor)
        for factor in newFactorList:
            factors.remove(factor)
        if newFactorList:
            print("Eliminating variable {0}: ".format(var))
            # print("FactorList: ")
            # for f in newFactorList:
            #     print(f.variables)
            # print()

            productFactor = newFactorList[0]
            for factor in newFactorList[1:]:
                productFactor = multiply(productFactor, factor)

            resultFactor = sumout(productFactor, var)

            printFactor(resultFactor)
            factors.append(resultFactor)

    result = factors[0]
    for factor in factors[1:]:
        result = multiply(result, factor)

    normalizedFactor = normalize(result)
    return normalizedFactor

def printFactor(factor):
    for var in factor.variables:
        print(var + '       ', end=' ')
    print('P')
    for key, value in factor.CPT.items():
        for i in range(len(key)):
            print(key[i] + '         ', end=' ')
        print(value)
    print()

def main():
    f1 = Factor(['Trav'], {'t': 0.05, 'f': 0.95})
    f2 = Factor(['Fraud', 'Trav'], {'tt': 0.01, 'tf': 0.004, 'ft': 0.99, 'ff': 0.996})
    f3 = Factor(['FP', 'Trav', 'Fraud'], {'ttt': 0.9, 'ttf': 0.9, 'tft': 0.1, 'tff': 0.01, 'ftt': 0.1, 'ftf': 0.1, 'fft': 0.9, 'fff': 0.99})
    f4 = Factor(['IP', 'OC', 'Fraud'], {'ttt': 0.02, 'ttf': 0.01, 'tft': 0.011, 'tff': 0.001, 'ftt': 0.98, 'ftf': 0.99, 'fft': 0.989, 'fff': 0.999})
    f5 = Factor(['OC'], {'t': 0.6, 'f': 0.4})
    f6 = Factor(['CRP', 'OC'], {'tt': 0.1, 'tf': 0.001, 'ft': 0.9, 'ff': 0.999})


    print('Q2b(1)*************************')
    factorList = [f1, f2]
    factorList1 = copy.deepcopy(factorList)
    printFactor(f1)
    printFactor(f2)
    order1 = ['Trav', 'FP', 'IP', 'OC', 'CRP']
    evidenceList1 = {}
    result1 = inference(factorList1, 'Fraud', order1, evidenceList1)
    print('Pr(Fraud): ')
    printFactor(result1)

    print('Q2b(2)*************************')
    factorList = [f1, f2, f3, f4, f5, f6]
    factorList2 = copy.deepcopy(factorList)
    printFactor(f1)
    printFactor(f2)
    printFactor(f5)
    order2 = ['Trav', 'FP', 'IP', 'OC', 'CRP']
    evidenceList2 = {'FP': 't', 'IP': 'f', 'CRP': 't'}
    result2 = inference(factorList2, 'Fraud', order2, evidenceList2)
    print('Pr(Fraud|fp, ~ip, crp) =  ')
    printFactor(result2)

    print('Q2c*************************')
    factorList = [f1, f2, f3, f4, f5, f6]
    factorList3 = copy.deepcopy(factorList)
    printFactor(f2)
    printFactor(f5)
    order3 = ['Trav', 'FP', 'IP', 'OC', 'CRP']
    evidenceList3 = {'FP': 't', 'IP': 'f', 'CRP': 't', 'Trav': 't'}
    result3 = inference(factorList3, 'Fraud', order3, evidenceList3)
    print('Pr(Fraud|fp, ~ip, crp, trav): ')
    printFactor(result3)

    print('Q2d(1)*************************')
    factorList = [f1, f2, f3, f4, f5, f6]
    factorList4 = copy.deepcopy(factorList)
    printFactor(f1)
    printFactor(f3)
    printFactor(f5)
    printFactor(f6)
    order4 = ['Trav', 'FP', 'IP', 'OC', 'CRP']
    evidenceList4 = {'IP': 't'}
    result4 = inference(factorList4, 'Fraud', order4, evidenceList4)
    print('Pr(Fraud|ip): ')
    printFactor(result4)

    print('Q2d(2)*************************')
    factorList = [f1, f2, f3, f4, f5, f6]
    factorList5 = copy.deepcopy(factorList)
    printFactor(f1)
    printFactor(f3)
    printFactor(f5)
    printFactor(f6)
    order5 = ['Trav', 'FP', 'IP', 'OC', 'CRP']
    evidenceList5 = {'IP': 't', 'CRP': 't'}
    result5 = inference(factorList5, 'Fraud', order5, evidenceList5)
    print('Pr(Fraud|ip, crp): ')
    printFactor(result5)

main()
