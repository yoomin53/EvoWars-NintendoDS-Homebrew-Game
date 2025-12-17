# EvoWars:AlienAssualt 
## Nintendo DS Homebrew Commercial Showcase 
> Commercial Nintendo DS homebrew action shooter
> featuring era-based progression and real-time combat.

![Gameplay Preview](media/gameplay.gif)

This project focuses on
AI behavior modeling, system design, and data-driven decision-making
under strict hardware constraints.

---
## 🎯 Project Purpose

This repository exists as a **supporting portfolio project** demonstrating applied AI and system-level thinking through a **real, constrained production environment**.

The project explores how:
- Intelligent enemy behavior
- Difficulty balancing
- Real-time decision systems  

can be designed with a Behaviour tree.


---

## 🕹️ Project Overview

- **Platform:** Nintendo DS (Homebrew)
- **Genre:** Cover-based shooter inspired by *NIKKE-style mechanics*
- **Core Theme:** Alien combat with tactical positioning
- **Status:** Commercial release planned (homebrew market)

Due to commercial considerations, the **source code is private**.  
This repository instead documents **design decisions, trade-offs, and analytical reasoning**.

---

## 🤖 AI & Decision System Design

Enemy Behavior Modeling
	•	Finite State Machines (FSM) combined with weighted scoring
	•	Action selection influenced by:
	•	Player exposure duration
	•	Distance to target
	•	Enemy health state
	•	Controlled stochastic variation

Conceptual model:

Action Score =
  0.4 × Distance Factor +
  0.3 × Player Exposure Time +
  0.2 × Enemy Health +
  0.1 × Random Noise

This approach ensures:
	•	Predictable baseline behavior
	•	Non-repetitive encounters
	•	Low computational overhead

⸻

Cover-Based Combat Logic
	•	Player state abstraction:
	•	Cover
	•	Aim
	•	Reload
	•	Exposed
	•	Enemy aggression dynamically responds to player risk state
	•	Encourages tactical timing rather than brute-force output

The system parallels policy-driven decision logic often used in applied AI systems.

⸻

Data-Driven Difficulty Design

Gameplay balance was tuned using offline simulations and synthetic player modeling, compensating for the lack of large-scale runtime telemetry.

Included analyses:
	•	Player reaction-time distributions
	•	Enemy spawn density vs. failure probability
	•	Difficulty curve stabilization

These analyses demonstrate how data science techniques can guide system tuning even in resource-limited environments

---

## Performance & Constraint-Aware Engineering

Key constraints:
	•	~4MB system memory
	•	Limited CPU throughput
	•	Fixed frame budget

Engineering responses:
	•	Deterministic AI update cycles
	•	Memory-aligned data layouts
	•	Frame-budgeted logic evaluation
	•	VRAM-conscious asset management

Every feature was evaluated for cost vs. behavioral benefit, mirroring real-world AI system optimization.

⸻

Repository Structure

├── docs/
│   ├── architecture.md
│   ├── nds-constraints.md
│   ├── ai-behavior-design.md
│   ├── game-system-design.md
│   └── performance-optimization.md
│
├── data-analysis/
│   ├── player-simulation.ipynb
│   ├── difficulty-balancing.ipynb
│   └── spawn-rate-analysis.ipynb
│
└── media/
    ├── gameplay.gif
    ├── boss-fight.mp4
    └── debug-overlay.png


⸻

Commercial & IP Notice

This project is intended for commercial distribution within the homebrew ecosystem.

To protect intellectual property:
	•	Source code is not publicly available
	•	Design documentation and analytical artifacts are shared instead

⸻

Summary

This project demonstrates:
	•	AI behavior modeling under extreme constraints
	•	Data-informed system tuning without heavy ML dependencies
	•	Real-time decision systems with predictable performance

It serves as a supporting example of applied AI engineering, complementing larger or more ML-focused portfolio projects.
