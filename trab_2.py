# Trabalho 2 de Sistemas Operacionais
# Alunos: José Victor Medeiros e Lucas Clemente

import operator
import copy

# Monitora a existência de tarefas pendentes.
def monitor(pending_jobs):
	pending = 0
	for i in pending_jobs:
		pending += i

	if (pending):
		return True
	return False

# Contabiliza ocorrências de tempo perdido.
def timeLost(job_lower, index, time_lost):
	event_index = [i for i, e in enumerate(job_lower) if e == index]
	if (len(event_index) > 1):
		if(event_index[-1] != event_index[-2] + 1):
			time_lost += event_index[-1] - event_index[-2] - 1

	return time_lost

# Lista as tarefas pendentes.
def remainingList(jobs, current, t_left):
	remaining = []
	for i in range(len(jobs)):
		if(int(jobs[i][0]) <= current and t_left[i] != 0):
			remaining.append(i)

	return remaining

# Contabiliza troca de contexto em caso de ocorrência.
def contextSwitchCouter(jobs, counter):
	if(len(jobs) > 1):
		if(jobs[-1] != jobs[-2]):
			counter += 1
	return counter

# Define valor inicial e final da tarefa atual.
def setBeginEnd(index, t_left, t_begin, t_end, current):
		if(t_begin[index] > current):
			t_begin[index] = current

		t_left[index] -= 1

		if(t_left[index] == 0):
			t_end[index] = current + 1

		current += 1

		return t_left, t_begin, t_end, current

# Calculo da média de time de execução e espera de tarefas.
def timeAvg(t_begin, t_end, entr_time, qtd_jobs, time_lost = 0):
	exec_time = 0.0
	waiting_time = 0.0

	for i in range(qtd_jobs):
		exec_time +=  float(t_end[i]) - float(entr_time[i][0])
		waiting_time += float(t_begin[i]) - float(entr_time[i][0])

	exec_avg = exec_time / qtd_jobs
	waiting_avg = (waiting_time + time_lost) / qtd_jobs

	return exec_avg, waiting_avg

# Pega os valores de uma string obtida no arquivo.
def getValues(list):
	values = []

	for i in range(len(list)):
		values.append(list[i].strip())

	return values

# jobs os valores obtidos do arquivo.
def getParam(file_name):
	with open(file_name, "r") as file:
		qtd_jobs = file.readline().strip()						# Quantidade de tarefas.
		entrance_time = getValues(file.readline().split(" "))	# Instantes de ingresso.
		execution_time = getValues(file.readline().split(" "))	# times de execução previstos.
		priority = getValues(file.readline().split(" "))		# Prioridades das tarefas

	return list(zip(entrance_time, execution_time, priority))

# Imprime os resultados.
def printResult(alg_name, t_exec, t_waiting, cont_switch, jobs):
    print("Alg. Escalonamento: %s" % (alg_name))
    print("Tempo médio de execução: %.1f" % (t_exec))
    print("Tempo médio de espera: %.1f" % (t_waiting))
    print("Troca médio de contexto: %d" % (cont_switch))
    print("Tempo total: %d" % (jobs))
    print("\n##############################\n")

########################################################################################################

# Algoritmo First Come First Served.
def FCFS(jobs):
	jobs.sort(key=operator.itemgetter(0))  #Ordena a jobs de tarefas por ordem de chegada.

	qtd_jobs = len(jobs)
	t_begin = []
	t_end = []
	time = 0

	for i in range(qtd_jobs):
		t_begin.append(time) 				# Insere o time inicial da tarefa.
		time += int(jobs[i][1]) 			# Calcula time de término da tarefa.
		t_end.append(time)					# Insere o time de término da tarefa.

	# Calcula a média de time e espera das tarefas.
	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, jobs, qtd_jobs)

	printResult("FCFS", t_exec_avg, t_waiting_avg, qtd_jobs-1, time)

# Shortest Job First.
def SJF(jobs):
	jobs.sort(key=operator.itemgetter(1))		# Ordena as tarefas por ordem de chegada.
	copy_jobs = copy.copy(jobs)

	qtd_jobs = len(jobs)						# Número de tarefas.
	t_begin = []
	t_end = []
	time = 0

	while(jobs):
		for i in range(qtd_jobs):
			if(int(jobs[i][0]) <= time):		# Verifica se o tempo do algoritmo é o menor.
				t_begin.append(time) 			# Insere o tempo inicial da tarefa.
				time = time + int(jobs[i][1])	# Calcula o tempo de término da tarefa.
				t_end.append(time)				# Insere o tempo de término da tarefa.
				jobs.pop(i)						# Remove a tarefa da lista.
				break

	# Calcula a média de tempo e espera das tarefas.
	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, copy_jobs, qtd_jobs)

	printResult("SJF", t_exec_avg, t_waiting_avg, qtd_jobs-1, time)


# Short Remaining Time First.
def SRTF(jobs):
	copy_jobs = copy.copy(jobs)
	qtd_jobs = len(jobs)

	t_begin = [18000] * qtd_jobs # Tempo no qual a tarefa iniciou sua execução, podendo ser um número grande.
	t_end = [18000] * qtd_jobs	 # Tempo no qual a tarefa terminou sua execução, também podendo um número grande.
	t_left = [] 				 # Armazena o tempo que falta para concluir cada tarefa.

	context_switch = 0			 # Contador de troca de contexto.
	job_lower = []				 # Armazena a tarefa com o menor tempo de sobra.
	time_lost = 0				 # Armazena o tempo perdido entre duas execuções sem tarefas.

	current = 0					 # Salva o tempo atual de execução.

	# Inicia a lista de tarefas a serem executadas.
	for i in range(qtd_jobs):
		t_left.append(int(jobs[i][1]))

	while(monitor(t_left)):

		remaining = remainingList(copy_jobs, current, t_left)

		# Seleciona a tarefa com menos tempo sobrando.
		lower = 100000
		index = 180000
		for j in remaining:
			if(lower > t_left[j]):
				lower = t_left[j]
				index = j
		job_lower.append(index)

		context_switch = contextSwitchCouter(job_lower, context_switch)

		time_lost = timeLost(job_lower, index, time_lost)

		t_left, t_begin, t_end, current = setBeginEnd(index, t_left, t_begin, t_end, current)

	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, copy_jobs, qtd_jobs, time_lost)

	printResult("SRTF", t_exec_avg, t_waiting_avg, context_switch, current)

# Prioridade cooperativa.
def PRIOc(jobs):
	jobs.sort(key=operator.itemgetter(2),reverse=1)
	copy_jobs = copy.copy(jobs)
	qtd_jobs = len(jobs)

	t_begin = [18000] * qtd_jobs # Tempo no qual a tarefa iniciou sua execução, podendo ser um número grande.
	t_end = [18000] * qtd_jobs	 # Tempo no qual a tarefa terminou sua execução, também podendo um número grande.

	current = 0					 # Salva o tempo atual de execução.

	performed_jobs = [500]*qtd_jobs

	while(monitor(performed_jobs)):

		remaining = remainingList(copy_jobs, current, performed_jobs)

		# Sai do loop caso não haja tarefas pendentes.
		if(len(remaining) == 0):
			current += 1
			break

		first_pending = remaining[0]			# Seleciona a primeira tarefa da lista de pendentes.
		t_begin[first_pending] = current		# Insere o tempo inicial da tarefa.
		current += int(jobs[first_pending][1])	# Incrementa o tempo de execução atual.
		t_end[first_pending] = current			# Estabelece o tempo atual como de término da tarefa.
		performed_jobs[first_pending] = 0		# Tira a tarefa da lista de pendentes.

	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, copy_jobs, qtd_jobs)

	printResult("PRIOc", t_exec_avg, t_waiting_avg, qtd_jobs-1, current)

# Prioridade preemptiva.
def PRIOp(jobs):
	copy_jobs = copy.copy(jobs)
	qtd_jobs = len(jobs)

	t_begin = [18000] * qtd_jobs # Tempo no qual a tarefa iniciou sua execução, podendo ser um número grande.
	t_end = [18000] * qtd_jobs	 # Tempo no qual a tarefa terminou sua execução, também podendo um número grande.
	t_left = [] 				 # Armazena o tempo que falta para concluir cada tarefa.

	bigger_job = []			 	 # Guarda a tarefa executada com maior prioridade.

	context_switch = 0			 # Contador de troca de contexto.
	time_lost = 0				 # Armazena o tempo perdido entre duas execuções sem tarefas.

	current = 0					 # Salva o tempo atual de execução.

	for i in range(qtd_jobs):
		t_left.append(int(jobs[i][1]))

	while(monitor(t_left)):

		remaining = remainingList(copy_jobs, current, t_left)

		bigger = 0
		index = 180000
		for j in remaining:
			if(bigger < int(jobs[j][2])):
				bigger = int(jobs[j][2])
				index = j
		bigger_job.append(index)

		time_lost = timeLost(bigger_job, index, time_lost)

		context_switch = contextSwitchCouter(bigger_job, context_switch)

		t_left, t_begin, t_end, current = setBeginEnd(index, t_left, t_begin, t_end, current)

	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, copy_jobs, qtd_jobs, time_lost)

	printResult("PRIOp", t_exec_avg, t_waiting_avg, context_switch, current)

# Prioridade dinâmica.
def PRIOd(jobs):
	copy_jobs = copy.copy(jobs)
	qtd_jobs = len(jobs)

	t_begin = [180000] * qtd_jobs # Tempo no qual a tarefa iniciou sua execução, podendo ser um número grande.
	t_end = [180000] * qtd_jobs	 # Tempo no qual a tarefa terminou sua execução, também podendo um número grande.
	t_left = [] 				 # Armazena o tempo que falta para concluir cada tarefa.

	younger_job = []			 # Guarda a tarefa executada mais "nova".
	dynamic = []

	context_switch = 0			 # Contador de troca de contexto.
	time_lost = 0				 # Armazena o tempo perdido entre duas execuções sem tarefas.

	current = 0					 # Salva o tempo atual de execução.

	for i in range(qtd_jobs):
		t_left.append(int(jobs[i][1]))
		dynamic.append(int(jobs[i][2]))

	while(monitor(t_left)):

		remaining = remainingList(copy_jobs, current, t_left)

		menor = 0
		index = 180000
		for j in remaining:
			if(menor < dynamic[j]):
				menor = dynamic[j]
				index = j
		younger_job.append(index)
		dynamic[index] = int(jobs[index][2])

		time_lost = timeLost(younger_job, index, time_lost)

		if(len(younger_job) > 1):
			if(younger_job[-1] != younger_job[-2]):
				context_switch += 1
				for i in remaining:
					if(i != index):
						dynamic[i] += 1

		t_left, t_begin, t_end, current = setBeginEnd(index, t_left, t_begin, t_end, current)

	t_exec_avg, t_waiting_avg = timeAvg(t_begin, t_end, copy_jobs, qtd_jobs, time_lost)

	printResult("PRIOd", t_exec_avg, t_waiting_avg, context_switch, current)

########################################################################################################

print("\n##############################\n")

param_list = getParam("entrada.txt")
FCFS(param_list)
SJF(param_list)

param_list = getParam("entrada.txt")
SRTF(param_list)
PRIOc(param_list)
PRIOp(param_list)

param_list = getParam("entrada.txt")
PRIOd(param_list)
