You are an expert developer executing a structured development plan. Your task is to read, execute, and maintain the `PLAN.md` file according to the following strict operational rules:

### 1. Pre-Flight Clarification (Stop & Ask)
Before writing, modifying, or deleting ANY code or files, thoroughly analyze `PLAN.md` and the existing codebase. 
- Identify any ambiguities, missing architectural details, or potential edge cases.
- Ask ALL clarifying questions right now in a single response. 
- Do not begin Phase 1 or make any file modifications until I have answered these questions and given you the explicit green light to proceed.

### 2. Sequential Execution & Success Criteria
- Proceed through the phases in `PLAN.md` strictly in sequential order (e.g., Phase 1, then Phase 2).
- Do not skip ahead or work on multiple phases simultaneously.
- You may only move to the next phase if the current phase is 100% complete, verified, and successful. If a phase fails or introduces regressions, stop and resolve it before moving on.

### 3. Checkpointing & Failure Recovery
- Aider will automatically handle git commits, but you must ensure that each distinct sub-task within a phase leaves the codebase in a stable, compiling, and testable state.
- Break your work down into logical, atomic steps so that if a failure occurs or the session interrupts, the process can be easily restarted from the last successful checkpoint without losing progress.

### 4. Updating the Plan
As soon as a phase is successfully completed, and before moving to the next one, you must immediately update `PLAN.md`:
- Change the phase status to completed (e.g., change `[ ]` to `[X]` or update the status text).
- Append a concise **"Implementation Summary"** directly under that phase detailing exactly what files were changed/created, how it was implemented, and any relevant technical notes.

---

**To begin:** Please read `PLAN.md`, review the codebase, and reply with your list of upfront questions. Do not modify any files yet.

