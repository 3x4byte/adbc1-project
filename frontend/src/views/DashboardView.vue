<template>
  <CoreLayout>
    <template #default>
      <h1>Dashboard</h1>
      <br>
      <div class="container-vertical">
        <VueDatePicker
          v-model="dateRange"
          range time-picker-inline
          :dark="useDarkTheme"
          :disabled="isLoading" />
        <VSelect
          v-model="selectedEsp"
          :options="availableEsps"
          :disabled="isLoading" />
        <VSelect
          v-model="selectedDataType"
          :options="dataTypes"
          :disabled="isLoading" />
      </div>
      
      <p class="hint">
        {{ hintMessage }}
      </p>

      <div v-if="isLoading" class="loading-container">
        <div class="loading-spinner"></div>
      </div>

      <LineChart :chart-data="clone(chartData)"/>
    </template>
  </CoreLayout>
</template>

<script>
import rfdc from 'rfdc';

import CoreLayout from '@/components/CoreLayout.vue';
import LineChart from '@/components/LineChart.vue';

import VueDatePicker from '@vuepic/vue-datepicker';
import '@vuepic/vue-datepicker/dist/main.css'
import VSelect from 'vue-select';
import 'vue-select/dist/vue-select.css';

export default {
  name: 'ChartView',
  components: {
    CoreLayout,
    LineChart,
    VueDatePicker,
    VSelect,
  },
  data() {
    return {
      dateRange: null,
      selectedEsp: '',
      availableEsps: [],
      espMailIdMap: {},
      selectedDataType: 'Detailed',
      dataTypes: ['Detailed', 'Balanced', 'Summarized'],
      clone: rfdc(),
      useDarkTheme: window.matchMedia('(prefers-color-scheme: dark)').matches,
      isLoading: false,
      hintMessage: '',
      chartData: {
        labels: [],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            yAxisID: "temperature",
            tension: 0.4,
            pointRadius: 0,
            data: []
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            yAxisID: "humidity",
            tension: 0.4,
            pointRadius: 0,
            data: []
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            yAxisID: "air_pressure",
            tension: 0.4,
            pointRadius: 0,
            data: []
          },
        ],
      },
      chartOptions: {
        type: 'line',
        responsive: true,
        plugins: {
          legend: {
            display: true,
          },
        },
      },
    }
  },
  methods: {
    async getData(){
      this.hintMessage = '';
      if(!this.dateRange || !this.selectedEsp || !this.selectedDataType){
        this.hintMessage = 'Please select a date range, device, and data type.'
        return null;
      }

      this.isLoading = true;

      let dateRangeStart = new Date(this.dateRange[0]);
      let dateRangeEnd = new Date(this.dateRange[1]);

      let formattedStart = dateRangeStart.toISOString().slice(0, 19).replace('T', ' ');
      let formattedEnd = dateRangeEnd.toISOString().slice(0, 19).replace('T', ' ');
      let espId = this.espMailIdMap[this.selectedEsp]

      let list = []

      let _labels = [];
      let _temperatures = [];
      let _humidities = [];
      let _airPressures = [];
        
      try {
        if(this.selectedDataType === "Detailed"){
          list = await this.$pb.collection("data").getFullList({
            filter: `created > '${formattedStart}' && created < '${formattedEnd}' && iot_user = '${espId}'`,
            sort: 'created'
          });
          for (let item of list) {
          _labels.push(item.created)
          _temperatures.push(item.temperature)
          _humidities.push(item.humidity)
          _airPressures.push(item.air_pressure)
        }
        }else{
          if(this.selectedDataType === "Balanced"){
            list = await this.$pb.collection('data_acc_hourly').getFullList({
              filter: `org_time > '${formattedStart}' && org_time < '${formattedEnd}' && iot_user_id = '${espId}'`,
              sort: 'org_time'
            });
          }
          else if(this.selectedDataType === "Summarized"){
            list = await this.$pb.collection('data_acc_daily').getFullList({
              filter: `org_time > '${formattedStart}' && org_time < '${formattedEnd}' && iot_user_id = '${espId}'`,
              sort: 'org_time'
            });
          }

          for (let item of list) {
            _labels.push(item.org_time)
            _temperatures.push(item.temperature)
            _humidities.push(item.humidity)
            _airPressures.push(item.air_pressure)
          }
        }
        
        return {labels: _labels, temperatures: _temperatures, humidities: _humidities, airPressures: _airPressures}
      } catch (error){
        console.error("Error fetching data:", error)
        this.hintMessage = 'Error fetching data. Please try again later.';
        return null
      } finally{
        this.isLoading = false
      }
    },

    setData(labels, temperatures, humidities, airPressures){
      this.chartData.labels = labels;
      this.chartData.datasets[0].data = temperatures;
      this.chartData.datasets[1].data = humidities;
      this.chartData.datasets[2].data = airPressures;
    },

    appendData(label, temperature, humidity, airPressure){
      this.chartData.labels.push(label);
      this.chartData.datasets[0].data.push(temperature);
      this.chartData.datasets[1].data.push(humidity);
      this.chartData.datasets[2].data.push(airPressure);
    },
    
    clearData(){
      if(this.chartData.labels !== undefined){
        this.chartData.labels.length = 0;
        this.chartData.datasets.forEach((dataset) => (dataset.data.length = 0));
      }
    },

    async updateChart(){
      this.clearData();
      let data = await this.getData()
      if(data){
        this.setData(data.labels, data.temperatures, data.humidities, data.airPressures)
        this.hintMessage = ''
      }
        
    },

  },
  watch: {
    dateRange(){
      this.updateChart();
    },
    selectedEsp(){
      this.updateChart();
    },
    selectedDataType(){
      this.updateChart();
    }
  },
  async mounted() {
    let userID = this.$pb.authStore.record.id

    const espIdList = await this.$pb.collection("users").getFullList({
      filter: `id = '${userID}'`,
      fields: 'iot_devices'
    });

    for(let espId of espIdList[0].iot_devices){
      let response = await this.$pb.collection("iot_user").getFullList({
       filter: `id = '${espId}'`,
       fields: 'email'
      });
      let email = response[0].email;

      this.espMailIdMap[email] = espId;
      this.availableEsps.push(email);
    }

    this.selectedEsp = this.availableEsps[0];

    let now = new Date();
    let hourBefore = new Date();
    hourBefore.setHours(hourBefore.getHours() - 1);

    this.dateRange = [ hourBefore, now ];

    this.updateChart();
  }
}
</script>

<style scoped>
.container-vertical{
  display: flex;
  gap: 20px;
}

.dp__main{
  width: 350px;
}

.v-select{
  width: 200px;
}

.hint{
  color: orange;
  height: 20px;
}

.loading-container {
  display: flex;
  justify-content: center;
  align-items: center;
  position: absolute;
  top: -100px; 
  left: 50%;
  transform: translateX(-50%);
  z-index: 10;
  width: 100%;
  height: 100%;
}

.loading-spinner {
  border: 15px solid var(--color-background-mute);
  border-top: 15px solid var(--color-text);
  border-radius: 50%;
  width: 100px;
  height: 100px;
  animation: spin 2s linear infinite;
}

/* Spinner animation */
@keyframes spin {
  0% { transform: rotate(0deg); }
  100% { transform: rotate(360deg); }
}

</style>