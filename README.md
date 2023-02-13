# Zombie Survival Game: AI

This was an exam for Gameplay Programming. The task was to make an AI for the human agent. You have to take care of energy, health, stamina, exploration, inventory management and danger avoidance.

https://user-images.githubusercontent.com/42802496/216970709-17d77c47-f5ea-4ebd-ac8b-f7ab91d82b06.mp4

I opted for a behavior tree because of the simplicity.
![image2](https://user-images.githubusercontent.com/42802496/216971864-e69c17dd-47e8-447b-916c-b28a55f30df0.png)

For those that are not familiar with behavior trees, you start at the root node and execute each node under it starting from the left.
A selector node will try execute the first node under it on the left, if it succeeds it returns and ignore all other nodes. If it fails, it tries the next one under itself.
A sequence node will try and execute all nodes under itself from left to right, however, if one fails it returns failure.
A group node is a node I came up with to seperate inventory management and world interraction inside my tree. The group node will execute all nodes under itself regardless of outcome.

In hindsight, I should've spent more time on exploration and searching and less on inventory management. All in all, I feel very satisfied with the outcome considering this is my first experience with making a game AI.
