import sys
import time
from StcPython import StcPython

SUBSCRIBER_URL = 'http://localhost:3000/subs/1'
PORT_TX = '//10.28.233.81/1/1'
PORT_RX = '//10.28.233.85/1/1'

print 'Loading STC...'
stc = StcPython()

print 'Loading Config...'
stc.perform('LoadFromXml', filename='simple.xml')

stc.config('project1.port(1)', location=PORT_TX)
stc.config('project1.port(2)', location=PORT_RX)
print 'Bringing Ports %s and %s online...' % (PORT_TX, PORT_RX)
stc.perform('AttachPorts')

stc.subscribe(parent='project1', resultParent='project1',
              configType='analyzer',
              resultType='analyzerportresults',
              filterList='',
              viewAttributeList='totalframecount',
              interval=1)

print 'Subscribing to 1 property of AnalyzerPortResults on behalf of %s ...' % SUBSCRIBER_URL
stc.perform('SubscribePropertyChangeCommand', PropertyClassId='analyzerportresults',
            PropertyIdList=['analyzerportresults.totalframecount'],
            PublishUrl=SUBSCRIBER_URL,
            EnableSubClasses=True,
            IncludeResultParent=True)

print 'Starting Sequencer...'
stc.perform('SequencerStart')
print 'Test Running.'
stc.waitUntilComplete()
print 'Test Complete.'







