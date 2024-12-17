<template>
  <div>
    <h1>Willkommen im Dashboard!</h1>
    <button @click="logout">Abmelden</button>

    <VueDatePicker v-model="dateRange" @update:model-value="handleNewDate" range time-picker-inline />
    <br>
    <LineChart :chart-data="chartData"/>
  </div>
</template>

<script>
import LineChart from '@/components/LineChart.vue';

import VueDatePicker from '@vuepic/vue-datepicker';
import '@vuepic/vue-datepicker/dist/main.css'

export default {
  name: 'ChartView',
  components: {
    LineChart,
    VueDatePicker
  },
  data() {
    return {
      dateRange: null,
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
    async getData(start, end){
      let formattedStart = start.getUTCFullYear() + '-' 
                   + String(start.getUTCMonth() + 1).padStart(2, '0') + '-' 
                   + String(start.getUTCDate()).padStart(2, '0') + ' ' 
                   + String(start.getUTCHours()).padStart(2, '0') + ':' 
                   + String(start.getUTCMinutes()).padStart(2, '0') + ':' 
                   + String(start.getUTCSeconds()).padStart(2, '0');

      let formattedEnd = end.getUTCFullYear() + '-' 
                   + String(end.getUTCMonth() + 1).padStart(2, '0') + '-' 
                   + String(end.getUTCDate()).padStart(2, '0') + ' ' 
                   + String(end.getUTCHours()).padStart(2, '0') + ':' 
                   + String(end.getUTCMinutes()).padStart(2, '0') + ':' 
                   + String(end.getUTCSeconds()).padStart(2, '0');

      const list = await this.$pb.collection("data").getFullList({
        filter: `created > '${formattedStart}' && created < '${formattedEnd}'`
      });

      for (const item of list) {
        this.appendData(item.created, item.temperature, item.humidity, item.air_pressure)
      }

    },

    appendData(label, temperature, humidity, air_pressure){
      this.chartData = {
        labels: [...this.chartData.labels, label],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            yAxisID: "temperature",
            tension: 0.4,
            pointRadius: 0,
            data: [...this.chartData.datasets[0].data, temperature],
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            yAxisID: "humidity",
            tension: 0.4,
            pointRadius: 0,
            data: [...this.chartData.datasets[1].data, humidity],
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            yAxisID: "air_pressure",
            tension: 0.4,
            pointRadius: 0,
            data: [...this.chartData.datasets[2].data, air_pressure],
          },
        ],
      };

    },
    
    clearData(){
      this.chartData = {
        labels: [],
        datasets: [
          {
            label: "temperature",
            backgroundColor: '#ff0000',
            borderColor: '#ff0000',
            data: []
          },
          {
            label: "humidity",
            backgroundColor: '#0000ff',
            borderColor: '#0000ff',
            data: []
          },
          {
            label: "air_pressure",
            backgroundColor: '#00ff00',
            borderColor: '#00ff00',
            data: []
          },
        ],
      };

    },

    handleNewDate(modelDate){
      this.clearData()
      let start = new Date(modelDate[0])
      let end = new Date(modelDate[1])
      this.getData(start, end)
    },

    async logout() {
      await this.$pb.authStore.clear()
      this.$router.push('/login'); // Weiterleiten
    }

  },
  mounted() {

    let now = new Date()
    let hour_before = new Date()
    hour_before.setHours(now.getHours() - 1)

    this.dateRange = [ hour_before, now ]
    this.getData(hour_before, now);
  }
}
</script>
