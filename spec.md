# HTN Planner Plugin for Unreal Engine 5 - Technical Specification

## 1. System Overview

- A hierarchical task network (HTN) planning system implemented as a UE5 plugin
- Visual graph editor interface similar to UE5's behavior tree system
- Support for both full planning and partial planning
- Integration with UE5's existing AI systems including SmartObjectSystem

## 2. Core Components

### 2.1 World State Representation

- Blackboard-like approach similar to UE5 behavior trees
- Properties can be read/written by tasks and sensors

### 2.2 Task Hierarchy

- **Compound Tasks**: Blueprint-derived classes that can be extended in C++ or Blueprint
    - Can contain multiple methods representing different approaches
    - Methods implemented as separate nodes connected to compound tasks
    - Support for recursion with safeguards against infinite loops (max plan length limit)
- **Primitive Tasks**: Blueprint-derived classes similar to StateTreeNodeBlueprintBase
    - Contain operators (actual implementations of actions)
    - Include preconditions as visual graph of condition nodes
    - Define effects on world state
    - Support for expected effects to handle anticipated world state changes

### 2.3 Planner

- Depth-first search algorithm as described in the article
- Method Traversal Record (MTR) automatically tracked for plan prioritization
- Support for both full planning and partial planning modes

### 2.4 Plan Executor/Runner

- Executes primitive tasks in sequence
- Interfaces between sensors, goal setting, and planning
- Triggers replanning when necessary

### 2.5 Sensor System

- Runs at configurable time intervals
- Checks world state against planned tasks' preconditions
- Triggers replanning when preconditions are invalidated

## 3. Integration with UE5

### 3.1 Navigation

- Specific task nodes that trigger the navigation system
- HTN system remains separate from navigation implementation details

### 3.2 Perception & EQS

- Sensors integrate with UE5's perception and Environment Query System
- Update world state based on perception data

### 3.3 SmartObjectSystem

- Used for agent coordination
- Planners claim smart object handles to prevent other agents from interacting
- Smart objects can have sensors for additional coordination

## 4. Debugging Features

- Step-by-step planning visualization
- World state inspection at different planning stages
- Logging of planning process and decisions

## 5. Implementation Priorities

- Focus on correct functionality before optimization
- Support for recursion with safeguards against infinite recursion
- Clear separation of planning logic from execution details

## 6. User Experience

- Blueprint-based task creation
- Visual editor for task hierarchy and method organization
- Familiar interface for developers coming from behavior trees
