# Zombie Survival Game: AI

This was an exam for Gameplay Programming. The task was to make an AI for the human agent. You have to take care of energy, health, stamina, exploration, inventory management and danger avoidance.

https://user-images.githubusercontent.com/42802496/216970709-17d77c47-f5ea-4ebd-ac8b-f7ab91d82b06.mp4

I opted for a behavior tree over a FSM since I find it easier to grasp and is more readable to me. On top of that, it is easier to iterate on the decision making since you can easily move blocks around.
The biggest downside of a behavior tree is the amount of memory it takes and the fact that it doesn't really remember where it was.
![image2](https://user-images.githubusercontent.com/42802496/216971864-e69c17dd-47e8-447b-916c-b28a55f30df0.png)

For those that are not familiar with behavior trees, you start at the root node and execute each node under it starting from the left.
A selector node will execute all it's child nodes until one returns success.
A sequence node will execute all it's child nodes until one returns failure.
A group node is a node I came up with to seperate inventory management and world interraction inside my tree. The group node will execute all nodes under itself regardless of outcome.

The part I did best is inventory management. It's code is very readable, uses `std` and `stl` as much as possible. Next to that, I am very glad my agent has 100% shooting accuracy and is quite good at handling enemies (as long as it has a weapon).

In hindsight, I should've spent more time on exploration, danger handling and searching and less on inventory management. All in all, I feel very satisfied with the outcome considering this is my first experience with making a game AI.
