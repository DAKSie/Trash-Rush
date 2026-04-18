---
name: game-design-logic-improvements
description: 'Improve gameplay design and game logic quality. Use when tuning fairness, challenge curve, replayability, enjoyability, and performance-safe behavior while preventing logic regressions.'
argument-hint: 'Game mode, target player skill, pain points, and constraints (for example: "endless mode, casual players, early deaths, preserve controls")'
---

# Game Design And Logic Improvements

## Outcome
Produce concrete gameplay and logic changes that improve fairness, clarity, challenge curve, replay value, and overall enjoyability without breaking core controls, runtime stability, or frame-time expectations.

## When To Use
- Players report the game is too easy, too hard, or inconsistent.
- Score pacing, spawn rules, or penalties feel unfair.
- Progression or difficulty spikes feel abrupt.
- You need to improve game feel while preserving existing mechanics.
- You are preparing a gameplay tuning pass before release.

## Inputs To Collect
1. Current game mode and primary loop (start, action, fail/win, restart).
2. Target player profile (new, casual, experienced, speedrunner).
3. Main pain points from playtests (for example: cheap hits, boring midgame).
4. Non-negotiable constraints (controls, art direction, performance budget, deadline).
5. Tunable parameters (spawn rates, speed scaling, health, score multipliers, timers).
6. Success metrics (survival time, score distribution, retries per session, frustration rate, and fun rating).

## Procedure
1. Define the change goal.
- Pick one primary objective per pass (fairness, challenge, pacing, readability, or depth).
- Freeze unrelated goals to avoid noisy iterations.

2. Baseline current behavior.
- Record 3 to 5 representative play sessions.
- Capture key metrics: early-fail rate, median survival time, score growth slope, and common death causes.
- Identify where challenge diverges from intent.

3. Build a logic map.
- List key systems and interactions: player state, enemy/trash spawning, collision outcomes, score rules, and fail states.
- Mark deterministic rules versus random events.
- Identify high-impact control points for tuning.

4. Propose a minimal change set.
- Select 1 to 3 parameter or rule changes with clear hypotheses.
- Predict expected impact for each change before implementing.
- Prefer reversible, localized edits over broad rewrites.

5. Implement and guard behavior.
- Apply changes incrementally in gameplay logic code.
- Preserve invariants: no impossible states, no negative counters unless intentional, no duplicate state transitions.
- Keep input handling and rendering behavior stable unless explicitly part of the goal.

6. Playtest and compare.
- Re-run the same scenario set used for baseline.
- Compare metrics against hypotheses and acceptance targets.
- Keep changes that improve the target metric without introducing regressions.

7. Iterate or finalize.
- If targets are not met, adjust one variable at a time and retest.
- If targets are met, document final parameter values and rationale.

## Decision Points
- Is the issue primarily fairness or challenge?
  - Fairness: reduce unavoidable damage, add telegraphing, improve grace windows.
  - Challenge: increase complexity gradually, not via sudden raw speed spikes.
- Are failures caused by randomness more than player decisions?
  - Yes: constrain RNG ranges or add spawn safety rules.
  - No: focus on readability and mastery depth.
- Is pacing flat after the opening minute?
  - Yes: add staged escalation triggers and milestone variation.
  - No: keep current pacing and tune only local pain points.
- Do fixes require architecture changes?
  - Yes: isolate systems first, then tune.
  - No: keep changes parameter-driven for faster iteration.

## Completion Checks
- Core loop remains intact and understandable within one run.
- Difficulty curve matches target player profile for early, mid, and late segments.
- At least one primary metric improves measurably from baseline.
- Playtesters report improved enjoyability for the target player profile.
- Logic updates do not cause measurable frame-time regressions in normal gameplay.
- No new logic regressions in scoring, collisions, or fail/win conditions.
- Changes are documented with rationale and rollback notes.

## Deliverables
- Baseline vs post-change metric snapshot.
- Final tuned parameter list and rule changes.
- Short validation summary with known remaining risks.

## Example Prompts
- Improve early-game fairness in this raylib project without lowering overall challenge.
- Tune spawn and score logic so average session length reaches 90 seconds for casual players.
- Refactor and simplify collision and fail-state logic before adding a new enemy type.
