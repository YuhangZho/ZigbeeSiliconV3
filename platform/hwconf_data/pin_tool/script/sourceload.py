#!/usr/bin/env python3

import generator.sourcegen as sourcegen
import Studio
import sys

# Utilities for Python 3
if sys.version_info[0] == 3:
  basestring = (str, bytes)

def load_requirement(requirement, candidates, options, changeset):
  readonly = requirement.properties.get('readonly')
  if requirement.getType() in ['gpio'] and 'signal' not in requirement.getProperties():
    port = options.get('PORT')
    pin_number = options.get('PIN')
    if port and pin_number:
      for pin in candidates:
        if str(pin.getIndex()) == pin_number and str(pin.getPortBank().getName()) == port:
          requirement.setModule(pin)
          # TODO make requirement module selection readonly
          pass

  elif requirement.getType() in ['prs']:
    channel = options.get('CHANNEL')
    if channel:
      for prs in candidates:
        prs_channel = sourcegen.get_module_instance(str(prs.getName()))
        if prs_channel == channel:
          requirement.setModule(prs)
          # TODO make requirement module selection readonly

          if 'PORT' in options and 'PIN' in options:
            port = options.get('PORT')
            pin_number = options.get('PIN')
            selector = next(sourcegen.get_selectors(prs))
            route = selector.routes[0]
            for loc in route.locations:
              if str(loc.pin.getIndex()) == pin_number and str(loc.pin.getPortBank().getName()) == port:
                location_number = str(loc.locationNumber)

                if 'LOC' in options:
                  # Match against found location
                  # TODO enable this test
                  #assert(options.get('LOC') == location_number)
                  pass

                if selector.locationPropertyReference is not None:
                  changeset.append(Studio.halConfig.newPropertySetting(
                    selector.locationPropertyReference,
                    location_number
                  ))
                changeset.append(Studio.halConfig.newPropertySetting(
                  route.enablePropertyReference,
                  "Enabled"
                ))
                break
            else:
              pass
              #print("## ERROR ## Location mismatch for PRS requirement")
            if readonly:
              changeset.append(Studio.halConfig.newIsReadOnlySetting(
                selector.locationPropertyReference,
                True
              ))
              changeset.append(Studio.halConfig.newIsReadOnlySetting(
                route.enablePropertyReference,
                True
              ))
          break
      else:
        print("## Pin Tool Error ## PRS channel {} doesn't exist".format(channel))

  else:
    peripheral_name = options.get('PERIPHERAL')
    for peripheral in candidates:
      if peripheral.getName() != peripheral_name:
        continue

      if 'channel' in requirement.properties:
        channels = requirement.properties['channel']
        if isinstance(channels, basestring):
          channels = [channels]
        else:
          channels = list(channels)
        for req_index, channel_name in enumerate(channels):
          channel_number = options.get('{}_CHANNEL'.format(channel_name))
          if channel_number is not None:
            changeset.append(Studio.halConfig.newPropertySetting(
              peripheral.ref('requirement.{}.channel'.format(req_index)),
              channel_number
            ))
            if readonly:
              changeset.append(Studio.halConfig.newIsReadOnlySetting(
                peripheral.ref('requirement.{}.channel'.format(req_index)),
                True
              ))

            signal_prefixes = peripheral.getProperty('channel.signal').split(',')
            for j, signal_prefix in enumerate(signal_prefixes):
              if j != 0:
                prefix = '{}_{}'.format(channel_name, signal_prefix)
              else:
                prefix = channel_name

              signal = signal_prefix + channel_number
              for selector in sourcegen.get_selectors(peripheral):
                for route in selector.routes:
                  if route.getName() == signal:
                    update_signal(options, selector, route, prefix, prefix, changeset, readonly)
      
      if 'signal' in requirement.properties:
        signals = requirement.properties['signal']
        if isinstance(signals, basestring):
          signals = [signals]
        else:
          signals = list(signals)

        for signal in signals:
          if signal.startswith('('):
            # Strip parantheses indicating that the signal is optional
            signal = signal[1:-1]

          for selector in sourcegen.get_selectors(peripheral):
            for route in selector.routes:
              if route.getName() == signal:
                location_prefix = selector.getName().split('_')[-1]

                update_signal(options, selector, route, signal, location_prefix, changeset, readonly)

      requirement.setModule(peripheral)
      # TODO make requirement module selection readonly
      break
    else:
      print("## ERROR ## Unknown peripheral {}".format(peripheral_name))


def update_signal(options, selector, route, pin_prefix, location_prefix, changeset, readonly):
  port = options.get('{}_PORT'.format(pin_prefix))
  pin_number = options.get('{}_PIN'.format(pin_prefix))

  for loc in route.locations:
    if str(loc.pin.getIndex()) == pin_number and str(loc.pin.getPortBank().getName()) == port:
      location_number = str(loc.locationNumber)

      if '{}_LOC'.format(location_prefix) in options:
        # Match against found location
        # TODO enable this test
        #assert(options.get('LOC') == location_number)
        pass

      if selector.locationPropertyReference is not None:
        changeset.append(Studio.halConfig.newPropertySetting(
          selector.locationPropertyReference,
          location_number
        ))
      changeset.append(Studio.halConfig.newPropertySetting(
        route.enablePropertyReference,
        "Enabled"
      ))
      break
  else:
    print("## ERROR ## Location mismatch for {} - {}".format(pin_prefix, selector.getName()))

  if readonly:
    changeset.append(Studio.halConfig.newIsReadOnlySetting(
      selector.locationPropertyReference,
      True
    ))
    changeset.append(Studio.halConfig.newIsReadOnlySetting(
      route.enablePropertyReference,
      True
    ))


def load_options_from_requirement(requirement):
  output_section = '[{}_{}]'.format(requirement.getType().upper(), requirement.getName().upper())
  in_section = False
  in_block_comment = False
  options = {}
  for line in requirement.getDefineContent():
    if '${}'.format(output_section) in line:
      in_section = True
    elif '{}$'.format(output_section) in line:
      in_section = False
      break
    elif in_block_comment:
      try:
        comment_end = line.index('*/')
        in_block_comment = False
        line = line[comment_end+2:]
      except ValueError:
        # No block comment end
        pass
    
    if in_section and not in_block_comment:
      # Find first comment
      line_comment_start = 10000
      block_comment_start = 10000
      try:
        block_comment_start = line.index('/*')
        line_comment_start = line.index('//')
      except ValueError:
        pass
      
      if block_comment_start < line_comment_start:
        # Strip block comments
        try:
          comment_start = line.index('/*')
          comment_end = None
          try:
            comment_end = line.index('*/')
          except ValueError:
            # No end found
            comment_end = False
          
          if comment_end is False:
            line = line[:comment_start]
            in_block_comment = True
          else:
            line = line[:comment_start] + ' ' + line[comment_end+2:]
        except ValueError:
          # No block comment found
          pass

      # Strip line comments
      try:
        comment_start = line.index('//')
        line = line[:comment_start]
      except ValueError:
        # No line comment found
        pass

      # Strip whitespace, only consider #defines
      line = line.strip()
      if not line.startswith('#'):
        continue
      line = line[1:].lstrip().split()

      if len(line) != 3:
        continue

      if line[0] != 'define':
        continue

      if not line[1].startswith(requirement.getName().upper()):
        continue

      options[line[1][len(requirement.getName())+1:]] = line[2]
  return options
