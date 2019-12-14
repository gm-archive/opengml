## Tasks
- [ ] Optimizing bytecode compiler of any sort.
- [ ] @-prefixed multiline strings
- [ ] more sophisticated string copy-on-write so that `x += " blah"` is not copied.
- The following asset types:
    - [ ] Timelines
    - [ ] Paths
    - [ ] **Datafiles**
- [ ] **D&D programming support** (action\_)
- [ ] **Listener events** (keyboard presses, collision, etc.)
- [ ] Render batching
- The following library function categories :
    - audio:
        - audio\_
        - sound\_
    - camera\_
    - current\_/date\_
    - d3d\_light
    - d3d\_model
    - platform-specific:
        - device\_
        - gesture\_
        - clickable\_
        - os\_
    - ds\_grid\_
    - ds\_priority\_
    - ds\_queue
    - ds: csv
    - effects
    - social/commercial
        - achievement\_
        - facebook\_
        - highscore\_
        - iap\_
        - steam\_
    - xboxlive\_
    - font\_
    - gamepad\_
    - joystick\_
    - game\_ save/load
    - get\_ (async)
    - gpu\_
    - http\_
    - **keyboard\_ (some remaining things)**
    - **instance\_,position\_ (some remaining things)**
    - matrix\_
    - hashing:
        - md5\_
        - sha1\_
    - **mouse\_**
    - motion:
        - move\_...
        - mp\_grid
        - mp\_linear
        - mp\_potential
    - network\_
    - particles
    - physics
    - screen\_save
    - shader
    - skeleton\_
    - tilemap\_
    - timelines
    - url\_
    - **view\_**
    - **window\_**

## Behaviour

These need to be checked to see what the intended behaviour is.

- after destroying an instance, can it be collided with? `place_meeting(x, y, destroyed_instance)`? `instance_position()` at the location of the dead instance?

## Iterative builds

- `.manifest.ogmc` containing the following:
    - list of all local, global, globalvar variables in order (the namespaces)
    - (\*) the list of files which contain globalvar, enum, or macro definitions (needed to check for a change, and also for ease of rebuilding)
- full rebuild occurs if any `identifier` (variable, function name, constant) node could be reinterpreted:
    - *any* changes to enums or macros
    - *any* changes to globalvars, asset indices, bytecode indices, or room instances (i.e. if any room file changed.)
    - these can be detected by checking (a) if any of the files (\*) have been updated, or if while preprocessing any of the new/changed files a global accumulation occurs (macro/enum/globalvar).
- minor rebuild:
    - expands namespaces from the ones in the manifest
    - updates manifest
    - may assign different variable->id mappings than a clean rebuild, but who cares?
    - what needs to be rebuilt?
        - all files which are modified since last time!