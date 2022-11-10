# 조명 처리 모델 (Lighting Model)

## 난반사 성분 (Diffuse Component)
### 거친 표면의 성질을 표현하는 성분
- 시야 독립적 (view-independent)

### 난반사 조명 처리식 (색상 제외)   
- 난반사만 일어날 수 있는 표면으로 가정   
- 반사된 빛은 표면 법선 벡터와 광원 벡터 사이의 코사인 값에 의해 결정됨 (Lambert 법칙)   
- $i_{diff} = \textbf{n} \cdot \textbf{l} = cos \phi$
    - if $\phi > \pi / 2$, then $i_{diff} = 0 $
    - 광선 간 거리가 _l_ 인 경우, 표면에서 그 거리는 $ l /cos \phi$
### 난반사 조명 처리식 (색상 포함)
- $\textbf{i}_{diff} = max((\textbf{n} \cdot \textbf{l}), 0)\textbf{m}_{diff}\otimes \textbf{s}_{diff}$
    - $\textbf{m}_{diff}$: 물질의 난반사 색상, $\textbf{s}_{diff}$: 광원의 난반사 색상, $\otimes$: 성분곱 기호(hadamard product symbol)

----------------------------
## 정반사 성분 (Specular Component)
### 하이라이트를 생성함으로써 반짝거리는 표면을 구현하는 성분
- 하이라이트는 관측자가 표면의 굴곡을 이해할 수 있도록 함, 광원의 방향과 위치를 결정하는 데 도움을 줌
- 시야 종속적 (view-dependent)

### 하이라이트 표현식 (그래픽스 하드웨어 모델: Phong 정반사 조명 처리식)
- $i_{spec} = (\textbf{r} \cdot \textbf{v})^{m_{shi}} = (cos\rho)^{m_{shi}}$
    - $\textbf{r} = 2(\textbf{n} \cdot \textbf{l})\textbf{n} - \textbf{l}$
    - $\textbf{v}$: 표면 점 $\textbf{p}$로부터 관측자를 향하는 벡터, $\textbf{r}$: 법선 벡터 $\textbf{n}$을 기준으로 광원 벡터 $\textbf{l}$의 반사 벡터, $m_{shi}$: 물질 표면 광택도
- 물리학 원리에 근거한 항을 추가하여 변형하여 사용하기도 한다. ex) 거칠기, 그림자, 표면 마스킹, Fresnel 반사도 등

### 하이라이트 표현식 (Blinn-Phong | OpenGL, DirectX에서 사용)
- $i_{spec} = (\textbf{n} \cdot \textbf{h})^{m_{shi}} = (cos\theta)^{m_{shi}}$
    - $\textbf{h} = {\textbf{l} + \textbf{v} \over || \textbf{l} + \textbf{v} ||}$ , $\textbf{h}$는 점 $\textbf{p}$를 포함하면서 광원으로 나온 빛을 관측자의 눈으로 완전하게 반사해주는 평면의 법선 벡터
    - API에서 사용하는 이유는 반사벡터를 계산할 필요가 없어 속도가 향상하기 때문이다.

### 두 표현식 사이의 근사식
- $(\textbf{r} \cdot \textbf{v})^{m_{shi}} = (\textbf{n} \cdot \textbf{h})^{4m_{shi}}$

### 정반사 조명 처리식 (Blinn-Phong 색상 포함)
- $ \textbf{i}_{spec} = max((\textbf{n} \cdot \textbf{h}), 0)^{m_{shi}}\textbf{m}_{spec} \otimes \textbf{s}_{spec}$
    - 픽셀별 셰이딩을 사용할 때, 실루엣 에지에서 $\textbf{n} \cdot \textbf{l}$ 값이 0보다 작아져 $i_{spec}$값이 0으로 떨어지고 부자연스러운 선이 생기는 현상 발생

----------------------------
## 주변광 성분 (Ambient Component)
### 간접 조명 효과를 표현하기 위해 사용되는 성분
- 재질 상수와 광원 상수가 간단하게 결합된 상수항
- 광원 쪽을 향하고 있지 않은 면들도 희미한 색상을 띄게 해줌
- 대부분의 API는 전역적 주변광 값을 제공
- 광원의 반대쪽 면에 대해 주변광 성분만 이용할 경우 동일한 색상을 가지며 3차원 입체 효과가 사라지는 문제가 발생

### 주변광 조명 처리식
- $\textbf{i}_{amb} = \textbf{m}_{amb} \otimes \textbf{s}_{amb}$

#### 4번 문제에 대한 대안으로 제시된 주변광 구현법
- 채움 조명 처리(fill lighting) 구현, 이 경우 과장된 효과를 막기 위해 정반사 성분을 끈다.
- 의도적으로 하나의 광원을 배치 (모든 물체가 적어도 하나의 직접 조명을 받도록 구현)
- 관측자 위치에 점 광원을 부착

----------------------------
## 조명 처리식 (Lighting Equation)
### 지역 조명 처리 모델(local lighting model)
- 조명 효과가 광원으로부터 나온 빛에만 영향을 받는 모델

### 전체 조명식
- $\textbf{i}_{total} = \textbf{a}_{glob} \otimes \textbf{m}_{amb} + \textbf{m}_{emi} + c_{spot}(d(\textbf{i}_{diff} + \textbf{i}_{spec}) + \textbf{i}_{amb})$
    - $\textbf{a}_{glob}$: 전역 주변 광원 매개변수, $\textbf{m}_{amb}$: 주변광 물질 매개변수, $\textbf{m}_{emi}$: 물질 방사 매개변수
- $c_{spot} = max (-\textbf{l} \cdot \textbf{s}_{dir}, 0)^{s_{exp}}$
    - $\textbf{l}$: 광원 벡터, $s_{dir}$: 집중 조명 광선의 방향, $s_{exp}$: 감쇠(fall-off) 조절을 위한 누승(exponentiation) 인자
    - 집중 조명광이 아닌 경우 $c_{spot} = 1$
- $ d = {1 \over s_{c} + s_{l}||\textbf{s}_{pos} - \textbf{p}|| + s_{q}||\textbf{s}_{pos} - \textbf{p}||^2}$
    - $ ||\textbf{s}_{pos} - \textbf{p}|| $는 광원의위치 $\textbf{s}_{pos}$로부터 셰이딩 될 점 $\textbf{p}$까지의 거리
    - $s_c$: 정적 감쇠 효과 조절항, $s_l$: 선형 감쇠 효과 조절항, $s_q$: 이차 감쇠 효과 조절항
    - 물리학적으로 올바른 거리 감쇠 효과는 $s_c = 0$, $s_l = 0$, $s_q = 1$

### 다중 광원에 대한 조명식 (n개의 광원)
- $\textbf{i}_{total} = \textbf{a}_{glob} \otimes \textbf{m}_{amb} + \textbf{m}_{emi} + \overset{n}{\underset{k = 1}{\sum}} c_{spot}^k(d^k(\textbf{i}_{diff}^k + \textbf{i}_{spec}^k) + \textbf{i}_{amb}^k)$
- 다중 광원으로 인하여 빛의 강도의 합이 [0, 1]을 초과하는 경우가 발생할 경우 해결 방법
    - 값 고정(clamping)
    - 색채의 max 값을 기준으로 scaling
    - 가장 큰 영향을 주는 성분을 일정 범위로 제한

