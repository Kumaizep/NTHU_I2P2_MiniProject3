# NTHU I2P2 Mini Project 3 - AlphaOthello
Design and implement an A.I. that can play the boardgame Othello. 

Intro
---
In this mini project, we want to design AI to play othello boardgames. To beats the baselineAIs offered by TA, I tried several algrolithms to implement it. And since baseline 2~5 are deterministic (i.e. same board must get same move), I think the strategy of this AI can be judged from the game against random. Although The final goal is to defeat all baslines, but I would tend to judge the strength of AI based on the results of baseline1.

Pure value table
---
### Intro
It is a vary simple way of thinking to play othello. Since the chess position would affect the situation of whole game, for example, corners of board are most important postions of the game. Althrough it would not must to win the game as we get corners, but it would have a high chance of winning. According to this idea, we can create a value table that means the used to represent the quality of the location. And we choose the spot with highest value.
### Implementation
I tried several different tables, and the following 2 tables have better result.
- #### Value table 1 : 
    ```
    {99,  -8,  8,  6,  6,  8,  -8, 99}
	{-8, -24, -4, -3, -3, -4, -24, -8}
	{ 8,  -4,  7,  4,  4,  7,  -4,  8}
	{ 6,  -3,  4,  0,  0,  4,  -3,  6}
	{ 6,  -3,  4,  0,  0,  4,  -3,  6}
	{ 8,  -4,  7,  4,  4,  7,  -4,  8}
	{-8, -24, -4, -3, -3, -4, -24, -8}
	{99,  -8,  8,  6,  6,  8,  -8, 99}
    ```
- #### Value table 2 : 
    ```
    {20, -3, 11,  8,  8, 11, -3, 20}
	{-3, -7, -4,  1,  1, -4, -7, -3}
	{11, -4,  2,  2,  2,  2, -4, 11}
	{ 8,  1,  2, -3, -3,  2,  1,  8}
	{ 8,  1,  2, -3, -3,  2,  1,  8}
	{11, -4,  2,  2,  2,  2, -4, 11}
	{-3, -7, -4,  1,  1, -4, -7, -3}
	{20, -3, 11,  8,  8, 11, -3, 20}
    ```
They all start with same thinks, the corner is the best, the positions near corner are ㄊorst(C-square/X-square), boundary are not bad, etc.
### Game result
- #### Value table 1 win rate(100 games for every situation) : 
    |       | Baseline1 | Baseline2 | Baseline3 | Baseline4 | Baseline5 |
    | ----- | --------- | --------- | --------- | --------- | --------- |
    | Sente | 78%       | 100%      | 100%      | 100%      | 0%        |
    | Gote  | 76%       | 100%      | 100%      | 0%        | 0%        |

- #### Value table 2 win rate(100 games for every situation) : 
    |       | Baseline1 | Baseline2 | Baseline3 | Baseline4 | Baseline5 |
    | ----- | --------- | --------- | --------- | --------- | --------- |
    | Sente | 87%       | 100%      | 100%      | 100%      | 0%        |
    | Gote  | 89%       | 100%      | 0%        | 0%        | 0%        |
### Summary
From the result, obviously table1 has better results than table2 (in the case of baseline 1) and both not too worse, but when gaming with baseline 2~5, it is not good strage as gote. And through I can adjust the value let them can gaming as gote, but the biggest problem is that baseline 5 is too powerful, and it may be difficult to beat it by pure value table alone.

AlphaBeta pruning / MiniMax
---
### Intro
Since Othello is a zero-sum game, it can be assumed that both players choose the move that is most beneficial to them every time, and this is also the most unfavorable choice for the other side. So we can use AlphaBeta pruning / MiniMax algrolithm to simulate the gaming and choose the best move.
### Implementation
Regarding the way to implement MiniMax, the first thing to consider is the simulation depth. Of course, the deeper it is, the more system resources need to be used. Although increasing the depth is generally beneficial to the simulation accuracy (if the heuristic function can correctly reflect the situation), but since Minimax is a deterministic algorithm, blindly increasing the depth may not help defeat baselines, so I initially set the depth roughly at 10.

Secondly, DFS will be used to enter the lower layer. If it is the final game or reaches the specified depth, the heuristic value will be returned.

Finally, compare the values obtained from deeper layer. If the depth is our round, we choose the largest, otherwise we choose the smallest and return it.

About calculate the heuristic value, we need to design a heuristic function. I considered the factors affecting the game can be divided into board factors and environmental factors. The board factors I choose include number, position(3 kind), and status. Environmental factors include sente, gote, endgame, and non-endgame. So my heuristic function will have 4 sets of 5 parameters.

Initially, the parameters I chose were quantity+1, +30 at the corner, +3 outside the border, -5 beside the border, +20 if it is stable. Because choosing them is just a guess, of course it cannot bring perfect results (beating all beselines), but the win rate against random is not so low, so I think the direction chosen is roughly correct. 
Then I made a program specifically for automatically changing parameters and testing games (and modified the player AI to make it easy to change parameters).
```
auto_para:
    for p1, p2, p3, p4, p5 in 0..10
        write_parameter(p1, p2 * 10, p3 -5, (p4 - 10) * 2, p5 * 10) 
        execute(main, playerAI, baselines)
        output game_results
    end for
```
Although this program can help me save a lot of time, it would be a waste of time to find the parameters that can beat all the baselines in this way. So I selected a few groups from its test results that can beat most of the baselines parameters and manually adjusted them. However, not every group can be adjusted to win all the baselines. After trying a few sets, I found a set of parameters that can win baseline 2,3,4,5, but the match with baseline 1 can only achieve a winning rate of about 75%. Obviously, this set of parameters does not constitute a good game strategy in general, but a special strategy for baseline AIs.Finally, I found a set of parameters that could win baseline 2 ,3,4,5 and have a 99% win rate against baseline 1.

At this time, I noticed the problem: the influence of the sequence and the endgame on the game, in fact, the effect of the endgame strategy change is not great, but the proper modification of the original parameters can make the endgame more beneficial. The influence of the sequence of moves is relatively large. Due to the parity of the number of positions on the board and the advantages and disadvantages of positions, there are certain differences in the strategies required for the sequence of moves. Therefore, I made changes to the original parameters and tested it with gote to win the baseline. 2,3,4,5, of course the baseline 1 game still has a 98% win rate.
```
{Quantity, CXline, Boundary, Corner, Stable}
Non-Endgame:
	Sente: {4, -30, 0, 60, 60}
	Gote : {6, -10, 0, 90, 50} 
Endgame:
	Sente: {4, -30, -10, 60, 60}
	Gote : {6, -4, 0, 90, 50}
```


### Game result
- #### ABP with 4 sets of 5 parameters win rate(100 games for every situation) : 
    |       | Baseline1 | Baseline2 | Baseline3 | Baseline4 | Baseline5 |
    | ----- | --------- | --------- | --------- | --------- | --------- |
    | Sente | 99%       | 100%      | 100%      | 100%      | 100%      |
    | Gote  | 98%       | 100%      | 100%      | 100%      | 100%      |
### Summary
After the game test, I decided that this is a strong enough game strategy. But as a game AI, it is not good enough, the main reason is that it still uses deterministic algorithms. For the same game, even if you lose multiple times, the selected moves will not change. I also tried MCTS when doing this Mini Project. In fact, I think this type is better, even if I can't stably beat the baselines (maybe I made a mistake).

MCTS
---
### Intro
### Implementation
### Game result
- #### simulate 0.95 secend win rate(100 games for every situation) : 
    |       | Baseline1 | Baseline2 | Baseline3 | Baseline4 | Baseline5 |
    | ----- | --------- | --------- | --------- | --------- | --------- |
    | Sente | --%       | --%       | --%       | --%       | --%       |
    | Gote  | --%       | --%       | --%       | --%       | --%       |

- #### simulate 9.95 secend win rate(100 games for every situation) : 
    |       | Baseline1 | Baseline2 | Baseline3 | Baseline4 | Baseline5 |
    | ----- | --------- | --------- | --------- | --------- | --------- |
    | Sente | --%       | --%       | --%       | --%       | --%       |
    | Gote  | --%       | --%       | --%       | --%       | --%       |
### Summary

