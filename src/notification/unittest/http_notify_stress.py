import sys
import time
from StcPython import StcPython

SUBSCRIBER_URL = 'http://localhost:3000/subs/1'
NUM_OBJECTS = 100
ITERATIONS = 10

print 'Loading STC...'
stc = StcPython()

print 'Creating %s Testables...' % NUM_OBJECTS
testables = []
for i in xrange(0, NUM_OBJECTS):
  testables.append(stc.create('testable', under='project1'))

print 'Creating %s Cars...' % NUM_OBJECTS
cars = []
for i in xrange(0, NUM_OBJECTS):
  cars.append(stc.create('car', under='project1'))

context = '{testid: id}'

print 'Subscribing to 8 properties of Testable on behalf of %s ...' % SUBSCRIBER_URL
stc.perform('SubscribePropertyChangeCommand', PropertyClassId='testable',
            PropertyIdList=['testable.mys8', 'testable.mys16', 'testable.mys32',
                            'testable.mys64', 'testable.myu8', 'testable.myu16',
                            'testable.myu32', 'testable.myu64'],
            PublishUrl=SUBSCRIBER_URL,
            Context=context)

print 'Subscribing to 1 property of Car on behalf of %s ...' % SUBSCRIBER_URL
stc.perform('SubscribePropertyChangeCommand', PropertyClassId='car',
            PropertyIdList=['car.mileage'],
            PublishUrl=SUBSCRIBER_URL,
            Context=context)

print 'Subscribing to 1 property of Scriptable on behalf of %s ...' % SUBSCRIBER_URL
stc.perform('SubscribePropertyChangeCommand', PropertyClassId='scriptable',
            PropertyIdList=['scriptable.name'],
            PublishUrl=SUBSCRIBER_URL,
            EnableSubClasses=True,
            Context=context)

start_time = time.clock()
print 'Test started.'
num = 1
mileage = 1
for i in xrange(0, ITERATIONS):
  for t in testables:
    stc.config(t, mys8=num, mys16=num, mys32=num, mys64=num,
               myu8=num, myu16=num, myu32=num, myu64=num)
    sys.stdout.write('.')
  for car in cars:
    stc.config(car, mileage=mileage, name='mycar_%s' % mileage)
    sys.stdout.write('.')
  print '\nIteration %s complete' % (i + 1)
  num = num + 1
  if num == 9:
    num = 1
  mileage = mileage + 1

# Wait for queued notifications to complete.
time.sleep(2)
end_time = time.clock()

print 'Done!'
print 'Took %s seconds' % (end_time - start_time)




