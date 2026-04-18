---
name: ui-ux-design-improvement
description: 'Improve UI/UX quality of existing or new interfaces. Use when refining layout, typography, color, motion, accessibility, responsiveness, and interaction clarity. Includes design-direction selection, implementation guidance, and quality checks.'
argument-hint: 'Target surface, constraints, and quality bar (for example: "landing page, preserve brand, mobile-first")'
---

# UI/UX Design Improvement

## Outcome
Produce a design improvement plan and concrete implementation changes that elevate usability and visual quality while respecting existing product constraints.

## When to Use
- Polish an existing UI that feels generic or inconsistent.
- Design a new interface with a clear visual direction.
- Improve usability, accessibility, and responsive behavior before release.
- Translate design critique into actionable implementation steps.

## Inputs To Collect
1. Target scope (screen, flow, or component set).
2. Constraints (brand rules, design system, tech stack, timeline).
3. Device priorities (mobile-first, desktop-first, both).
4. Preferred visual style direction (if known).
5. Success criteria (conversion, readability, task completion, delight).

## Procedure
1. Audit current experience.
- Identify top UX friction points and visual inconsistencies.
- Capture constraints and non-negotiables.

2. Choose visual direction.
- Define typography, color system, spacing rhythm, and mood.
- If an existing design system exists, preserve patterns and extend only where needed.
- If no design system exists, define a minimal reusable token set first.

3. Redesign structure and hierarchy.
- Rework information hierarchy, spacing, and grouping.
- Prioritize primary actions and reduce competing calls to action.
- Ensure strong visual scanning order on both mobile and desktop.

4. Improve interaction and motion.
- Add purposeful state feedback (hover, focus, loading, success, error).
- Use limited, meaningful animations (entrance, transition, emphasis), avoiding ornamental motion.

5. Accessibility and content pass.
- Verify contrast, keyboard focus visibility, semantics, and touch target sizes.
- Tighten labels, helper text, and empty/error states for clarity.

6. Implement and validate.
- Apply changes incrementally in code with reusable style tokens and components.
- Validate against acceptance checks and regressions.

## Decision Points
- Existing design language present?
  - Yes: preserve and refine.
  - No: establish base tokens and patterns first.
- Is the task conversion-critical?
  - Yes: prioritize clarity and call-to-action prominence.
  - No: prioritize comprehension and aesthetic cohesion.
- Is motion likely to distract users or harm performance?
  - Yes: reduce to essential transitions.
  - No: keep tasteful motion accents.
- Is style direction missing or vague?
  - Yes: propose two contrasting style directions and pick one before implementation.
  - No: proceed with the specified direction.

## Completion Checks
- Clear visual direction is documented and consistently applied.
- Layout hierarchy makes primary tasks obvious in under five seconds.
- Mobile and desktop both render correctly at common breakpoints.
- Interactive states exist for all actionable controls.
- Accessibility baseline is met (contrast, focus, keyboard flow, touch size).
- No regressions in existing core user flows.

## Deliverables
- A prioritized list of UI/UX issues with severity.
- A concrete implementation plan (tokens, components, screens).
- Updated UI code and a short validation summary.
