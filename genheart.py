import matplotlib.pyplot as plt
import numpy as np
import subprocess

DT = 0.001
G = 0.1
N = 10
HEIGHT = 20

def heart(u):
  x = 16*np.sin(u)**3
  y = 13*np.cos(u) - 5*np.cos(2*u) - 2*np.cos(3*u)  - np.cos(4*u)
  return np.array([x/10, y/10])


heartpts = heart(np.linspace(0, 2*np.pi, 150))
print heartpts.shape
bouncepts = heartpts[::,np.where(heartpts[1] < 0)].squeeze()
print bouncepts.shape

class mass(object):

    def __init__(self, x=0., y=0., m=1.):
        self.mass = m
        self.position = np.array([x, y])
        self.velocity = np.array([0., 0.])

    def update(self):
        self.velocity += np.array([0., -G])
        self.position += self.velocity*DT



def bounce(velocity, normal, coeff=1):
    u = normal * (np.dot(velocity, normal)/np.dot(normal, normal))
    w = velocity - u
    return coeff*w - coeff*u



class Sphere(mass):

    def __init__(self, r=0.2, *args, **kwargs):
      super(self.__class__, self).__init__(*args, **kwargs)
      self.r = r
      self.color = np.random.normal(loc=0, scale=0.1, size=(3,))

    def __repr__(self):
        return 'sphere at: ' + str(self.position)

    def update(self, spheres):
        super(self.__class__, self).update()
        d = (bouncepts.transpose() - self.position).transpose()
        dabs = np.sqrt(d[0]**2 + d[1]**2)
        # plt.plot(dabs); plt.show()
        # print 'dabs:', dabs
        sortidx = np.argsort(dabs)


        closest_point = (bouncepts.transpose()[sortidx])[0]
        distance = dabs[sortidx][0]

        if distance <= self.r:
            print 'bounce'
            plt.plot(*closest_point, marker='o', linestyle='', color='purple')
            normal = d.transpose()[sortidx][0]*-1 # we need wall to obj
            self.velocity = bounce(self.velocity, normal, 0.25)
            # u = normal * (np.dot(self.velocity, normal)/np.dot(normal, normal))
            # w = self.velocity - u
            # self.velocity = w - u

        if self.position[1] < closest_point[1]:
            self.velocity[1] = 0.
            self.position[1] = closest_point[1]

        for sphere in spheres:
            if self is sphere:
                pass
            else:
                v = self.position - sphere.position

                distance = np.sqrt(v[0]**2 + v[1]**2)
                if distance < (self.r + sphere.r):
                    deltav = self.velocity - sphere.velocity
                    self.velocity += bounce(deltav, v, 1)
                    # accell = intersection*0.25*v
                    # self.velocity += self.mass*accell

        if self.position[0]-self.r > max(heartpts[0]):
            self.velocity = bounce(self.velocity, np.array([-1., 0.]), 0.95)
        elif self.position[0]+self.r  < -max(heartpts[0]):
            self.velocity = bounce(self.velocity, np.array([1., 0.]), 0.95)

        self.velocity *= (1 - self.r/2.)

origin = np.random.normal

spheres = [Sphere(x=x, y=y, r=r) for x, y, r in np.array([np.random.normal(loc=0, scale=0.5, size=(N,)),
                                                          np.random.normal(loc=HEIGHT, scale=3, size=(N,)),
                                                          np.random.normal(loc=0.2, scale=0.03, size=(N,))]).transpose()]

print spheres

def render(i, spheres):
    p = subprocess.Popen('./a.out anim/im{}.ppm'.format(i), shell=True, stdin=subprocess.PIPE)
    c = []

    for sphere in spheres:
        x, y = sphere.position[0], sphere.position[1]
        color = np.array([0.8, 0.3, 0.3]) + sphere.color
        c.append("sphere\n")
        c.append("glass\n")
        c.append("{} {} {} {} {} {} {}\n".format(x, y, -1, sphere.r*2, *color))

    c.insert(0, '{}\n'.format(len(spheres)))
    p.communicate(''.join(c))


for i in range(200):

    render(i, spheres)

    # plt.plot(*zip(*[sphere.position.copy() for sphere in spheres]), marker='o', linestyle='', color='b')

    for sphere in spheres:
        for _ in range(200):
            sphere.update(spheres)

    if i%10 == 0:
        spheres += [Sphere(x=x, y=y) for x, y in np.array([np.random.normal(loc=0, scale=0.5, size=(N,)),
                                                          np.random.normal(loc=HEIGHT, scale=3, size=(N,))]).transpose()]




# def heart(u):
#   x = 16*np.sin(u)**3
#   y = 13*np.cos(u) - 5*np.cos(2*u) - 2*np.cos(3*u)  - np.cos(4*u)
#   return np.array([x/10, y/10])
#
# plt.plot(heartpts[0], heartpts[1], color='red')
#
#
# # plt.xlim([-10, 10])
# # plt.ylim([-10, 10])
# plt.show()
