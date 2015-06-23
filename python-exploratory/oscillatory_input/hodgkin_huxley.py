import scipy as sp
import pylab as plt
from scipy.integrate import odeint
import numpy as np

class HodgkinHuxley():
  """Full Hodgkin-Huxley Model implemented in Python"""
  
  def __int__(self):
    # membrane capacitance, in uF/cm^2
    self.C_m  =   1.0
    # Sodium (Na) maximum conductances, in mS/cm^2
    self.g_Na = 120.0
    # Postassium (K) maximum conductances, in mS/cm^2
    self.g_K  =  36.0
    # Leak maximum conductances, in mS/cm^2
    self.g_L  =   0.3
    # Sodium (Na) Nernst reversal potentials, in mV
    self.E_Na =  50.0
    # Postassium (K) Nernst reversal potentials, in mV
    self.E_K  = -77.0
    # Leak Nernst reversal potentials, in mV
    self.E_L  = -54.387
    # The time to integrate over
    self.t = np.linspace(0.0, 450.0, 0.01)
    print(self.t)
    self.Main()
  
  
  
  def alpha_m(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 0.1*(self.V+40.0)/(1.0 - sp.exp(-(self.V+40.0) / 10.0))

  def beta_m(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 4.0*sp.exp(-(self.V+65.0) / 18.0)

  def alpha_h(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 0.07*sp.exp(-(self.V+65.0) / 20.0)

  def beta_h(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 1.0/(1.0 + sp.exp(-(self.V+35.0) / 10.0))

  def alpha_n(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 0.01*(self.V+55.0)/(1.0 - sp.exp(-(self.V+55.0) / 10.0))

  def beta_n(self):
    """Channel gating kinetics. Functions of membrane voltage"""
    return 0.125*sp.exp(-(V+65) / 80.0)

  def I_Na(self):
    """
    Membrane current (in uA/cm^2)
    Sodium (Na = element name)
    |  :param V:
    |  :param m:
    |  :param h:
    |  :return:
    """
    return self.g_Na * self.m**3 * self.h * (self.V - self.E_Na)

  def I_K(self):
    """
    Membrane current (in uA/cm^2)
    Potassium (K = element name)
    |  :param V:
    |  :param h:
    |  :return:
    """
    return self.g_K  * self.n**4 * (self.V - self.E_K)
    
  #  Leak
  def I_L(self):
    """
    Membrane current (in uA/cm^2)
    Leak
    |  :param V:
    |  :param h:
    |  :return:
    """
    return self.g_L * (V - self.E_L)

  def I_inj(self):
    """
    External Current
    |  :param t: time
    |  :return: step up to 10 uA/cm^2 at t>100
    |           step down to 0 uA/cm^2 at t>200
    |           step up to 35 uA/cm^2 at t>300
    |           step down to 0 uA/cm^2 at t>400
    """
    return 10*(self.t>100) - 10*(self.t>200) + 35*(self.t>300) - 35*(self.t>400)

  @staticmethod
  def dALLdt(self):
    """
    Integrate
    |  :param X:
    |  :param t:
    |  :return: calculate membrane potential & activation variables
    """
    self.V, self.m, self.h, self.n = self.X

    self.dVdt = (self.I_inj() - self.I_Na() - self.I_K() - self.I_L()) / self.C_m
    self.dmdt = self.alpha_m()*(1.0-m) - self.beta_m()*self.m
    self.dhdt = self.alpha_h()*(1.0-h) - self.beta_h()*self.h
    self.dndt = self.alpha_n()*(1.0-n) - self.beta_n()*self.n
    return self.dVdt, self.dmdt, self.dhdt, self.dndt

  def Main(self):
    """
    Main demo for the Hodgkin Huxley neuron model
    """
    print(self.t)
    self.X = odeint(self.dALLdt, [-65, 0.05, 0.6, 0.32], self.t, args=(self,))
    self.V = self.X[:,0]
    self.m = self.X[:,1]
    self.h = self.X[:,2]
    self.n = self.X[:,3]
    self.ina = self.I_Na()
    self.ik = self.I_K()
    self.il = self.I_L()

    plt.figure()

    plt.subplot(4,1,1)
    plt.title('Hodgkin-Huxley Neuron')
    plt.plot(self.t, self.V, 'k')
    plt.ylabel('V (mV)')

    plt.subplot(4,1,2)
    plt.plot(self.t, self.ina, 'c', label='$I_{Na}$')
    plt.plot(self.t, self.ik, 'y', label='$I_{K}$')
    plt.plot(self.t, self.il, 'm', label='$I_{L}$')
    plt.ylabel('Current')
    plt.legend()

    plt.subplot(4,1,3)
    plt.plot(self.t, self.m, 'r', label='m')
    plt.plot(self.t, self.h, 'g', label='h')
    plt.plot(self.t, self.n, 'b', label='n')
    plt.ylabel('Gating Value')
    plt.legend()

    plt.subplot(4,1,4)
    plt.plot(self.t, self.I_inj(self.t), 'k')
    plt.xlabel('t (ms)')
    plt.ylabel('$I_{inj}$ ($\\mu{A}/cm^2$)')
    plt.ylim(-1, 40)

    plt.show()

#if __name__ == '__main__':
#  runner = HodgkinHuxley()
#  runner.Main()
