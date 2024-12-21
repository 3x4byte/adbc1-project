<template>
  <div>
    <h1>Willkommen im Dashboard!</h1>
    <button @click="logout">Abmelden</button>
    <div class="container-vertical">
      <VueDatePicker v-model="dateRange" range time-picker-inline :dark="useDarkTheme"/>
      <v-select v-model="selectedEsp" :options="availableEsps" />
    </div>
    <br>
    <LineChart :chart-data="clone(chartData)"/>
  </div>
</template>

<script>
import rfdc from 'rfdc';

import LineChart from '@/components/LineChart.vue';

import VueDatePicker from '@vuepic/vue-datepicker';
import '@vuepic/vue-datepicker/dist/main.css'
import VSelect from 'vue-select';
import 'vue-select/dist/vue-select.css';

export default {
  name: 'ChartView',
  components: {
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
      clone: rfdc(),
      useDarkTheme: window.matchMedia('(prefers-color-scheme: dark)').matches,
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
      if(!this.dateRange || !this.selectedEsp)
        return null;

      let dateRangeStart = new Date(this.dateRange[0]);
      let dateRangeEnd = new Date(this.dateRange[1]);

      let formattedStart = dateRangeStart.toISOString().slice(0, 19).replace('T', ' ');
      let formattedEnd = dateRangeEnd.toISOString().slice(0, 19).replace('T', ' ');
      let espId = this.espMailIdMap[this.selectedEsp]

      const list = await this.$pb.collection("data").getFullList({
        filter: `created > '${formattedStart}' && created < '${formattedEnd}' && iot_user = '${espId}'`,
        sort: 'created'
      });

      let _labels = [];
      let _temperatures = [];
      let _humidities = [];
      let _airPressures = [];
      for (const item of list) {
        _labels.push(item.created)
        _temperatures.push(item.temperature)
        _humidities.push(item.humidity)
        _airPressures.push(item.air_pressure)
      }
      return {labels: _labels, temperatures: _temperatures, humidities: _humidities, airPressures: _airPressures}
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
      }
        
    },

    async logout() {
      await this.$pb.authStore.clear()
      this.$router.push('/login'); // Weiterleiten
    }

  },
  watch: {
    dateRange(){
      this.updateChart();
    },
    selectedEsp(){
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
</style>