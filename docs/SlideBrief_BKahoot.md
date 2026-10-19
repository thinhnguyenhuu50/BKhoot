# BKahoot slide deck brief

**Topic:** how a three-member team designed and built the game, and the problems they solved on the way.

## How to use this brief

Build one slide for each numbered entry below, in order. Everything the slides need is in this brief; no other source is required.

- **Audience:** the instructor and classmates of an Embedded Systems course. They know microcontrollers, C and RTOS concepts, but have not seen this project.
- **Length:** 14 slides, plus 1 optional slide.
- **Language:** English.
- **Scope:** two parts only. Part 1 is the team's process. Part 2 is five problems and their solutions. Slide 2 gives just enough project context for Part 2 to make sense. Do not add a feature walkthrough or code listings.

**Rules**

- Team members appear as Member A, Member B and Member C. These are placeholders for the real names.
- Show task durations only. Do not put calendar dates on any slide.
- Use only the figures given in this brief. They are design constants or calculated values. Do not add measured results such as frame rate, latency or packet success rate.
- Each slide has a title that states its point, then at most five bullets, or one table, or one diagram.

**Look**

- Clean technical style with a light background and plenty of diagrams.
- Colour code, used consistently: Member A and the STM32 are blue, Member B and the ESP8266 are orange, Member C is green. Radio links are dashed lines.
- The four Kahoot answer colours can serve as accents: red `#E21B3C`, blue `#1368CE`, yellow `#D89E00`, green `#26890C`.

---

## Slide 1. Title

- Title: **BKahoot: an offline multiplayer quiz game on STM32F407 and ESP8266**
- Subtitle: How a team of three built it, and the problems we solved
- Footer: Embedded Systems course, Member A, Member B, Member C

## Slide 2. The project in one slide

- A Kahoot-style classroom quiz that needs no Internet, no router and no phones. Identical development kits talk to each other directly over ESP-NOW (2.4 GHz radio).
- Every kit runs the same firmware. The user picks a role at start-up: one Host and up to 20 Players.
- Inside each kit, an STM32F407 (Cortex-M4F at 168 MHz) runs FreeRTOS, the LVGL touch interface on a 240×320 LCD, and all the game logic.
- An ESP8266 with custom firmware is the radio co-processor. The two chips talk over UART at 115200 baud using a binary frame: `[0xAA][CMD][LEN][PAYLOAD up to 200 B][XOR checksum]`.
- Game flow: the Host opens a lobby, Players scan and join, the Host sends a question with four coloured answers and a countdown, Players answer, the Host scores and sends feedback, then a leaderboard and the next question.

**Visual:** block diagram. One Host kit in the centre and three or four Player kits around it, joined by dashed radio links labelled "ESP-NOW". An inset shows the inside of one kit: STM32 (blue) with LCD and touch panel, linked by UART to the ESP8266 (orange).

---

# Part 1. The team's process

## Slide 3. Team and roles

| Member | Role | Owns |
|---|---|---|
| **A** | System platform and integrator | Build system (CubeMX and CMake), clock, the FSMC bus for the LCD and external SRAM, DMA, FreeRTOS set-up, memory management, RS232 debug console, performance measurement |
| **B** | Communication | ESP8266 firmware, the UART frame protocol shared by both chips, ESP-NOW (broadcast, unicast, acknowledgement, drop detection), the STM32 receive and transmit paths |
| **C** | Application and interface | LCD and touch drivers with calibration, the LVGL port, 10 screens and their styles, the Host and Player state machines, the question bank and its code generator |

Closing line on the slide: the split follows the architecture's own seams (platform, radio link, application), so each member owns one layer and meets the others through a narrow interface.

## Slide 4. Development process: five phases, five milestones

Each phase has the same length (5 working days per member) and ends with integration on the `main` branch and a demo.

| Phase | Goal | Milestone demo |
|---|---|---|
| 1 | Foundation and specification | M1: the display renders and reacts to touch, RS232 logging works, protocol specification v1 is agreed |
| 2 | System infrastructure and chip-to-chip link | M2: STM32 and ESP8266 exchange checksummed frames, static screens navigate |
| 3 | First end-to-end game | M3: one Host and one Player play a full question |
| 4 | Reliability and feature completion | M4: all features work with three or more kits, feature freeze |
| 5 | Test, fix and deliver | M5: firmware v1.0, report, slides, demo video |

Add one line under the table: design came first. Phase 1 produced three design artefacts before feature code was written: the protocol specification (frame format and command table), wireframes of the 10 screens with their navigation flow, and the split of responsibilities between the two microcontrollers.

**Visual:** five chevrons in a row with a milestone diamond under each. Label each chevron "5 days". No dates.

## Slides 5 and 6. Task breakdown and durations

Slide 5 covers phases 1 to 3. Slide 6 covers phases 4 and 5 and shows the totals.

Durations are in working days (d).

| Phase | Member A (platform) | Member B (communication) | Member C (application and interface) |
|---|---|---|---|
| **1. Foundation and specification** | Build system, 168 MHz clock, RTOS heartbeat task: 2 d<br>FSMC bring-up for LCD and external SRAM: 2 d<br>RS232 debug log over UART with DMA: 1 d | ESP8266 toolchain, LED and UART echo tests: 2 d<br>Protocol specification v1: 1 d<br>Frame parser and builder shared by both chips: 2 d | LCD, touch and calibration-storage drivers: 3 d<br>LVGL port (display flush, touch input): 1 d<br>Wireframes of 10 screens and navigation flow: 1 d |
| **2. Infrastructure and link** | RTOS heap on external SRAM (linker section, allocator redirection, boot self-test): 3 d<br>RS232 command console, RTOS statistics, stack sizing: 2 d | ESP-NOW broadcast between two ESP8266 modules: 1 d<br>UART to ESP-NOW bridge firmware with log forwarding: 2 d<br>STM32 receive path (interrupt, parser, queue): 2 d | Shared styles and four static screens: 3 d<br>Touch debounce and coordinate filter: 1 d<br>Game logic skeleton (roles, states, interface): 1 d |
| **3. First end-to-end game** | Thread-safety review (display lock rule, resource ownership matrix, UART lock): 2 d<br>Crash diagnosis support (fault handler logging): 1 d<br>Draw buffer size and placement experiments: 2 d | Room discovery and join flow: 2 d<br>Question delivery and answer return: 2 d<br>Static message pool for the receive interrupt: 1 d | Host question screen and Player answer screen with countdown: 2 d<br>Host and Player state machines: 2 d<br>JSON question bank and build-time code generator: 1 d |
| **4. Reliability and features** | Persistent screens, with C: 2 d<br>Two draw buffers in external SRAM: 1 d<br>Heap and stack measurement, release build: 2 d | Switch from broadcast to acknowledged unicast: 2 d<br>Peer table, miss counter, dropped-player detection: 2 d<br>Host-assigned player IDs, rejoin keeps score: 1 d | Scoring and per-player feedback: 1 d<br>Leaderboard, final ranking and time-up screens: 2 d<br>Early end of question, Home button, interface polish: 2 d |
| **5. Test, fix and deliver** | Joint bug fixing: 2 d<br>Tests: boot, memory, long-run stability: 2 d<br>Report, slides, demo: 1 d | Joint bug fixing: 2 d<br>Tests: protocol, range, drop and rejoin, UART contention: 2 d<br>Report, slides, demo: 1 d | Joint bug fixing: 2 d<br>Tests: touch, screens, question bank: 2 d<br>Report, slides, demo: 1 d |
| **Total** | **25 d** | **25 d** | **25 d** |

Team total: 75 person-days, split equally.

**Visual:** a duration chart in three coloured lanes (A, B, C) and five phase columns. Each task is a bar whose length is proportional to its days, labelled with the task name and "2 d". No dates on the axis. Put the three 25 d totals at the right end of slide 6.

## Slide 7. How three people worked in parallel

- **Interfaces first.** The frame protocol and two call boundaries (the interface calls `game_*()`, the logic calls `gui_load_*()`) were fixed early, so each member could build against a stub.
- **One protocol source file for both chips.** The same `protocol.c` is compiled into the STM32 firmware and the ESP8266 firmware, so the two parsers can never disagree.
- **Dependencies tracked explicitly.** The FSMC and LCD work (A) came before the LVGL port (C). The protocol specification (B) came before the state machines (C). The external heap (A) came before the large draw buffers. The STM32 receive path (B) came before the join flow.
- **Rhythm.** A 15-minute planning meeting opened each phase. Work happened on feature branches, such as `esp8266`, which were merged into `main` after a review by another member. Each phase closed with an integration demo.
- **One shared debug tool.** An RS232 console shows the logs of both chips (the ESP8266 log is forwarded through the STM32) and the heap and stack statistics. It works on several kits at once without halting any CPU, which matters because a breakpoint on one kit desynchronises it from the others.

**Visual:** the three lanes from slides 5 and 6, reduced to boxes, with four arrows for the dependencies above.

---

# Part 2. Problems and solutions

Use the same layout on slides 9 to 13: **Symptom**, **Cause**, **Solution**, **Alternatives compared** (small table, chosen row highlighted), **Lesson** (one line at the bottom).

## Slide 8. Five problems at a glance

| # | Problem | Area | Solution in one line |
|---|---|---|---|
| 1 | Not enough RAM for the graphical interface | Memory | Move the whole RTOS heap to the 512 KB external SRAM and test that memory at every boot |
| 2 | Crash (HardFault) as soon as radio messages arrive | Interrupts | Stop allocating in the interrupt: use a static message pool and queue pointers |
| 3 | Jerky animations and slow screen changes | Graphics | DMA pixel transfer, two draw buffers, and screens that are built once and reused |
| 4 | Players silently miss questions | Wireless | Send to each player by unicast with a radio-level acknowledgement, and drop dead links after 3 misses |
| 5 | Frames lost when two tasks transmit at once | Concurrency | A mutex with priority inheritance around the UART transmit call |

## Slide 9. Problem 1: not enough RAM for the graphical interface

- **Symptom.** The STM32F407 has 128 KB of main SRAM. The interface alone needs two draw buffers (2 × 38,400 B = 76,800 B), the widgets of 10 screens kept alive, plus task stacks and RTOS objects. It does not fit.
- **Solution.** Put the entire FreeRTOS heap in the kit's external SRAM (512 KB at address `0x68000000`, on the FSMC bus):
  - a new memory region and a no-load section in the linker script hold a 512 KB heap array;
  - FreeRTOS `heap_5` is pointed at that array before any RTOS object is created;
  - the linker option `--wrap` redirects `malloc`, `free`, `realloc` and `calloc` to the RTOS heap, so LVGL and the C library share one heap with one set of statistics;
  - the interface task's 8 KB stack stays in internal SRAM because it is accessed constantly.
- **A bug on the way.** Two FSMC byte-lane pins (NBL0 and NBL1) were missing from the CubeMX configuration, so the LCD and the external SRAM misbehaved. After the fix the team added a boot self-test: two full write and read-back passes over all 512 KB (an address-derived pattern, then its inverse) before the RTOS may use the memory. The result and the first bad address go to RS232.
- **Cost accepted.** External SRAM is slower than internal SRAM and shares the FSMC bus with the LCD.

| Option | Verdict |
|---|---|
| RTOS heap in internal SRAM only | Too small for two draw buffers, 10 screens and the task stacks |
| Core-coupled memory (CCM, 64 KB) | Fast, but DMA cannot reach it, so it cannot hold draw buffers |
| Keep one small draw buffer | Fits, but the display gets jerky (see Problem 3) |
| **RTOS heap in external SRAM (chosen)** | Plenty of room and readable by DMA; slower access |

**Lesson:** never trust memory you have not tested; check it before the system depends on it.

**Visual:** memory map with four columns (Flash 1 MB, SRAM 128 KB, CCM 64 KB, external SRAM 512 KB) showing what lives where.

## Slide 10. Problem 2: HardFault as soon as radio messages arrive

- **Symptom.** Right after ESP-NOW and the game logic were integrated, the board crashed with a HardFault when frames arrived from the ESP8266.
- **Cause.** The UART receive interrupt called the FreeRTOS allocator for every received byte. The FreeRTOS heap is not safe to use from an interrupt.
- **Solution.** A static pool of 10 message slots. The frame parser writes each byte straight into the current slot. When a frame is complete, the interrupt puts only a 4-byte pointer on the queue and moves to the next slot. The game task drains the queue. Nothing is allocated, freed or copied.
- **Follow-up.** The HardFault handler now captures the stacked CPU registers, so a future fault points to the instruction that caused it.
- **Known limit.** The pool has as many slots as the queue (10), so a burst that arrives while the queue is full could overwrite a slot. A larger pool is planned.

| Option | Verdict |
|---|---|
| Allocate each message in the interrupt | Crashes (the original design) |
| Queue the whole message by value | Safe, but copies up to 202 B inside the interrupt for every frame |
| **Static pool and pointer queue (chosen)** | Safe in an interrupt, no copy, about 2 KB of fixed RAM |
| Byte ring buffer, parse in a task | Shortest interrupt, but needs a larger redesign; future work |

**Lesson:** an interrupt must not allocate or block; hand over a pointer to memory that already exists.

**Visual:** before and after. Before: interrupt, allocator, crash. After: interrupt, pool slot, pointer queue, game task. Shade the interrupt context and the task context differently.

## Slide 11. Problem 3: jerky animations and slow screen changes

- **Context.** The STM32F407 has no graphics accelerator, so LVGL draws every pixel in software.
- **Symptom.** Screen changes and the countdown animation were visibly jerky.
- **Solution, in four steps:**
  1. DMA copies pixels from the draw buffer to the LCD through the FSMC bus, so the CPU is free during the transfer.
  2. The draw buffer went from one buffer of 1/10 screen to two buffers of 1/4 screen (38,400 B each). The CPU renders into one buffer while DMA sends the other.
  3. Screens became persistent. Each of the 10 screens is built once and then only updated. Before, every screen change destroyed and rebuilt all its widgets.
  4. The interface task got the highest priority and yields every 5 ms so the other tasks still run.
- **Cost accepted.** More RAM stays in use permanently. This is what made the external heap of Problem 1 necessary.

| Option | Verdict |
|---|---|
| One small buffer in internal SRAM | Little RAM, but the CPU waits for every transfer and a frame needs many transfers |
| Full-frame buffer | 153,600 B, and its 76,800 pixels exceed the 65,535-item limit of one DMA transfer |
| **Two 1/4-screen buffers (chosen)** | Rendering and transfer overlap; each transfer (19,200 pixels) fits in one DMA run |
| Rebuild a screen on every change | Less RAM held, but slow changes and heap fragmentation |
| **Persistent screens (chosen)** | Fast changes and a stable heap; more RAM held |

**Lesson:** on a small microcontroller, smooth graphics come from where the memory sits and who moves it, not from raw CPU speed.

**Visual:** timing diagram with two rows, "CPU renders" and "DMA transfers", showing the overlap with two buffers. Add a small inset of the single-buffer case where the CPU waits.

## Slide 12. Problem 4: players silently miss questions

- **Symptom.** Questions were sent by broadcast. If a packet was lost, that player stayed on the old screen and the Host waited for an answer that could not come. A player who had powered off looked the same as a slow one.
- **Cause.** ESP-NOW broadcast frames are not acknowledged, so the sender learns nothing about delivery.
- **Solution.**
  - The Host's ESP8266 sends each question and each "time up" message by unicast to every joined player in turn. Unicast frames are acknowledged and retried by the radio's MAC layer, and a callback reports success or failure for each player.
  - A table of up to 20 peers keeps a miss counter per player. After 3 consecutive failures the peer is dropped and the STM32 is told, so the Host stops waiting for that player.
  - A player who rejoins from the same kit gets the same ID (P1, P2, …) and score back.
  - Room discovery still uses broadcast, because no peer addresses are known yet.
- **Cost accepted.** A question is transmitted once per player instead of once in total.

| Option | Verdict |
|---|---|
| Broadcast only | One transmission, simplest; no knowledge of delivery |
| Broadcast plus our own acknowledgement and retry | Delivery is known, but timeouts and retries must be designed and debugged by us |
| **Unicast with MAC-layer acknowledgement (chosen)** | Delivery is known for free from the radio; N transmissions per question |
| Wi-Fi access point with TCP | Reliable and phones could join, but needs association and an ESP8266 access point accepts only a handful of stations |

**Lesson:** on a radio link, "sent" does not mean "delivered"; choose a mechanism that tells you which one happened.

**Visual:** sequence diagram. The Host sends to three players; one is powered off. Show miss 1, miss 2, miss 3, then "player dropped" going up to the Host STM32.

## Slide 13. Problem 5: frames lost when two tasks transmit at once

- **How it was found.** Not by testing. In a thread-safety review the team listed every shared resource against every task and interrupt that touches it. The row for the UART transmitter to the ESP8266 had two writers and no protection.
- **Failure scenario (Host only).** The game task is in the middle of sending a short reply, for example a join acknowledgement. The Host user taps Start. The interface task has higher priority, pre-empts, and calls transmit. The HAL driver returns "busy" and the frame is discarded. The return value was ignored, so nothing was logged. Players never receive the question while the Host's countdown runs.
- **Why the driver's own busy flag is not a lock.** It is checked and then set in two separate steps, so two tasks can both pass the check.
- **Solution.** An RTOS mutex around the transmit call only; frame building and logging stay outside it. The mutex has priority inheritance, so a low-priority holder is boosted while the interface task waits. The lock order is fixed (display lock first, UART lock second, never the reverse), which rules out deadlock. A failed transmit is now logged.
- **Cost, calculated at 115200 baud.** The interface task waits at most about 1.5 ms, the time of the game task's longest frame (17 B). The game task may wait up to about 17.7 ms while a full 204 B question frame goes out.
- **Verification.** Stress test: press Start or Next while a player is joining or scanning, 20 times or more. Every player must receive the question and the log must show no failed transmit. *[Add the result here once the test has been run.]*

| Option | Verdict |
|---|---|
| **Mutex around the transmit call (chosen)** | Small change, priority inheritance; the caller still blocks during the send |
| Transmit queue owned by one task, sent with DMA | Callers never block, but more code and a full-queue case to handle; future work |
| Disable interrupts during the send | Up to 17.7 ms with interrupts off loses received bytes; not acceptable |
| Only one task is allowed to send | No lock needed, but interface events must be rerouted and replies get up to 100 ms slower |
| Retry when the driver reports busy | Busy-waiting, no guarantee, and frame order becomes unpredictable |

**Lesson:** every shared hardware resource needs an explicit owner or lock, and a driver's error code must never be ignored.

**Visual:** two sequence diagrams side by side with lifelines for game task, interface task and UART. Left, before: the second transmit returns "busy" and the frame is lost. Right, after: the interface task blocks on the mutex, the game task is boosted and finishes, then the interface task sends.

## Slide 14 (optional). Problem 6: several Hosts in the same room

Use this slide if time allows, or swap it for one of the five above.

- **Symptom.** Every lobby had the same name, so players could not tell rooms apart. Hosts answered a scan by broadcast, so an answer meant for one scanning player also appeared on every other scanning kit, and the same Host could be listed several times.
- **Solution.**
  - Self-numbering. A new Host first scans the air itself and listens for 500 ms. Existing Hosts answer with their names, and the new Host takes the lowest free number: "BKahoot Lobby 1", "BKahoot Lobby 2", and so on. It stays hidden from scans until it has a number.
  - A Host answers a scan by unicast to the kit that scanned.
  - The Player's list ignores a Host it already shows, matching by MAC address.

| Option | Verdict |
|---|---|
| Append part of the MAC address to the name | Unique with no negotiation, but unreadable for players |
| Let the Host user type a name | Friendly, but needs an on-screen keyboard on a 240×320 touch panel |
| **Hosts number themselves (chosen)** | Readable and automatic; costs a 500 ms wait when a lobby opens |

## Slide 15. Lessons learned

- An interrupt must not allocate or block. Give it memory that already exists and let a task do the work.
- Every shared hardware resource needs an owner or a lock. Listing resources against tasks found a bug that testing had not.
- Test hardware before depending on it. A boot self-test turns a mysterious crash into a one-line message.
- On a radio link, "sent" is not "delivered". Use acknowledgements you can observe.
- Memory placement is a performance decision: fast internal RAM for what is touched constantly, large external RAM for the rest.
- For the team: fixing the interfaces in the first phase is what let three people work in parallel for the next three.
